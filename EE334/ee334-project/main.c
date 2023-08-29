/**
 * @file main.c
 * @author Kevin Evans (kevin.h.evans@wsu.edu)
 * @brief Implements a branch-target-buffer BTB for a sample trace file.
 * @date 2022-04-07
 * 
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define BTB_SIZE 1024
#define GET_BTB_INDEX(pc) (((pc) & 0xFFC) >> 2)

enum state_machine_type {
    STATE_MACHINE_A,
    STATE_MACHINE_B,
    STATE_MACHINE_CLASS
};

enum prediction_type {
    NOT_TAKEN      = 0b11,
    WEAK_NOT_TAKEN = 0b10,
    WEAK_TAKEN     = 0b01,
    TAKEN          = 0b00
};

/**
 * @brief A struct to hold a branch target buffer entry. 
 */
struct btb_entry {
    uint32_t pc : 32;
    uint32_t target : 32;
    enum prediction_type prediction : 2;
};

/**
 * @brief Converts a prediction type to a string of binary
 * 
 * @param type The enum type
 * @return const char* String of binary
 */
const char *prediction_type_to_string(enum prediction_type type) {
    switch (type) {
        case TAKEN:
            return "00";
        case WEAK_TAKEN:
            return "01";
        case WEAK_NOT_TAKEN:
            return "10";
        case NOT_TAKEN:
            return "11";
    }
}

/**
 * @brief Converts a state machine type to a string
 */
const char *state_machine_type_to_string(enum state_machine_type type) {
    switch (type) {
        case STATE_MACHINE_A:
            return "A";
        case STATE_MACHINE_B:
            return "B";
        case STATE_MACHINE_CLASS:
            return "CLASS";
    }
}

/**
 * @brief Reads the next trace from the trace file.
 * 
 * @param fp The file pointer to the trace file.
 * @param peek If true, the next trace does not advance the file pointer.
 * @return uint32_t Returns the address of the next trace or 0 if there are no more traces.
 */
uint32_t read_trace_pc(FILE *fp, bool peek) {
    uint32_t pc;
    long pos = ftell(fp);
    int ret = fscanf(fp, "%x", &pc);

    if (ret == EOF || ret != 1) {
        return 0;
    }

    if (peek) {
        // rewind back to the original position
        fseek(fp, pos, SEEK_SET);
    }

    return pc;
}

/**
 * @brief Get the new prediction, using the class state machine
 * 
 * @param old_prediction The current prediction in the BTB
 * @param direction The direction to update it, can ONLY be TAKEN or NOT_TAKEN
 * @return enum prediction_type 
 */
enum prediction_type get_new_prediction_class(enum prediction_type old_prediction, enum prediction_type direction) {
    switch (old_prediction) {
        case TAKEN:
            return direction == TAKEN ? TAKEN : WEAK_TAKEN;
        case WEAK_TAKEN:
            return direction == TAKEN ? TAKEN : WEAK_NOT_TAKEN;
        case WEAK_NOT_TAKEN:
            return direction == TAKEN ? WEAK_TAKEN : NOT_TAKEN;
        case NOT_TAKEN:
            return direction == TAKEN ? WEAK_NOT_TAKEN : NOT_TAKEN;
    }
}

/**
 * @brief Get the new prediction, implementing state machine A
 * 
 * @param old_prediction The current prediction in the BTB
 * @param direction The direction to update it, can ONLY be TAKEN or NOT_TAKEN
 * @return enum prediction_type 
 */
enum prediction_type get_new_prediction_A(enum prediction_type old_prediction, enum prediction_type direction) {
    switch (old_prediction) {
        case TAKEN:
            return direction == TAKEN ? TAKEN : WEAK_TAKEN;
        case WEAK_TAKEN:
            return direction == TAKEN ? TAKEN : NOT_TAKEN;
        case WEAK_NOT_TAKEN:
            return direction == TAKEN ? WEAK_TAKEN : NOT_TAKEN;
        case NOT_TAKEN:
            return direction == TAKEN ? WEAK_NOT_TAKEN : NOT_TAKEN;
    }
}

/**
 * @brief Get the new prediction, implementing state machine B
 * 
 * @param old_prediction The current prediction in the BTB
 * @param direction The direction to update it, can ONLY be TAKEN or NOT_TAKEN
 * @return enum prediction_type 
 */
enum prediction_type get_new_prediction_B(enum prediction_type old_prediction, enum prediction_type direction) {
    switch (old_prediction) {
        case TAKEN:
            return direction == TAKEN ? TAKEN : WEAK_TAKEN;
        case WEAK_TAKEN:
            return direction == TAKEN ? TAKEN : WEAK_NOT_TAKEN;
        case WEAK_NOT_TAKEN:
            return direction == TAKEN ? TAKEN : NOT_TAKEN;
        case NOT_TAKEN:
            return direction == TAKEN ? WEAK_NOT_TAKEN : NOT_TAKEN;
    }
}

/**
 * @brief Updates a prediction in an BTB entry
 * 
 * @param entry The entry to update
 * @param direction The direction to update it, can ONLY be TAKEN or NOT_TAKEN
 */
void update_prediction(struct btb_entry *entry, enum prediction_type direction, enum state_machine_type state_machine) {
    enum prediction_type new_prediction;

    if (state_machine == STATE_MACHINE_A) {
        new_prediction = get_new_prediction_A(entry->prediction, direction);
    } else if (state_machine == STATE_MACHINE_B) {
        new_prediction = get_new_prediction_B(entry->prediction, direction);
    } else {
        new_prediction = get_new_prediction_class(entry->prediction, direction);
    }

    entry->prediction = new_prediction;
}

/**
 * @brief Reads through a trace file and generates a BTB table
 * 
 * @param fp The file to read from.
 * @param entries The table to fill.
 */
void generate_btb(FILE *fp, struct btb_entry entries[], enum state_machine_type state_machine) {
    uint32_t pc_current, pc_next;
    struct btb_entry *entry = NULL;
    int ic = 0,
        hit = 0,
        miss = 0,
        right = 0,
        wrong = 0,
        wrong_addr = 0,
        collision = 0,
        taken = 0;
    float hit_rate, prediction_accuracy, incorrect_address;

    printf("* using state machine: %s\n", state_machine_type_to_string(state_machine));

    while(pc_current = read_trace_pc(fp, false)) {
        ic++;

        entry = &entries[GET_BTB_INDEX(pc_current)];
        pc_next = read_trace_pc(fp, true);

        if (entry->pc == pc_current) {
            // is pc_current in btb? (prediction)
            hit++;

            switch(entry->prediction) {
                case TAKEN:
                case WEAK_TAKEN:
                    if (pc_next == entry->target) {
                        right++;
                        taken++;

                        update_prediction(entry, TAKEN, state_machine);
                    } else {
                        wrong++;
                        
                        if (pc_next != (pc_current + 4)) {
                            wrong_addr++;
                            taken++;
                            entry->target = pc_next;

                            update_prediction(entry, TAKEN, state_machine);
                        } else {
                            update_prediction(entry, NOT_TAKEN, state_machine);
                        }
                    }
                    break;

                case NOT_TAKEN:
                case WEAK_NOT_TAKEN:
                    if (pc_next == (pc_current + 4)) {
                        right++;

                        update_prediction(entry, NOT_TAKEN, state_machine);
                    } else {
                        wrong++;
                        taken++;

                        update_prediction(entry, TAKEN, state_machine);
                    }
                    break;
            }
            

        } else {
            // if pc_next == 0, we're at the end of the trace
            if (pc_next == 0) {
                break;
            }

            // no prediction
            if ((pc_current + 4) != pc_next) {
                // is pc_current a branch? (target)
                miss++;
                taken++;

                if (entry->pc) {
                    // entries[index] already exists! let's overwrite it
                    collision++;
                }
                
                // set initial prediction here
                // using state machine A, the initial state is TAKEN
                entry->pc = pc_current;
                entry->target = pc_next;

                if (state_machine == STATE_MACHINE_CLASS || state_machine == STATE_MACHINE_A) {
                    // state machine A and CLASS starts at 00
                    entry->prediction = TAKEN;
                } else {
                    // state machine B has a different initial state 01
                    entry->prediction = WEAK_TAKEN;
                }
            } else {
                // normal execution
                continue;
            }
        }
    }

    // done running through the trace; print all the stats:
    printf("IC = %d\n", ic);
    printf("hit = %d\n", hit);
    printf("miss = %d\n", miss);
    printf("right = %d\n", right);
    printf("wrong = %d\n", wrong);
    printf("taken = %d\n", taken);
    printf("collision = %d\n", collision);
    printf("wrong_addr = %d\n", wrong_addr);

    hit_rate = (float)hit / (hit + miss);
    printf("hit rate = %.2f%%\n", 100.0 * hit_rate);

    prediction_accuracy = (float)right / (hit);
    printf("prediction accuracy = %.2f%%\n", 100.0 * prediction_accuracy);

    incorrect_address = (float)wrong_addr / (wrong);
    printf("incorrect address = %.2f%%\n\n", 100.0 * incorrect_address);
}

/**
 * @brief Prints the BTB table to stdout.
 * 
 * @param entries The table to print.
 */
void print_btb(struct btb_entry entries[], int limit) {
    int i;
    int count = 0;

    // printf("index pc     target pred.\n");

    for (i = 0; i < BTB_SIZE; i++) {
        if (entries[i].pc) {
            // print out non-zero entries
            printf(
                "%5d %6X %6X %s\n",
                i,
                entries[i].pc,
                entries[i].target,
                prediction_type_to_string(entries[i].prediction)
            );
            // printf("%d\t%x\t%x\n", i, entries[i].pc, entries[i].target);
            count++;

            if (count >= limit) {
                break;
            }
        }
    }
}

extern int errno;

enum state_machine_type parse_state_machine_type(const char *str) {
    if (strcmp(str, "A") == 0) {
        return STATE_MACHINE_A;
    } else if (strcmp(str, "B") == 0) {
        return STATE_MACHINE_B;
    } else if (strcmp(str, "CLASS") == 0) {
        return STATE_MACHINE_CLASS;
    } else {
        fprintf(stderr, "ERROR: invalid state machine type: %s\n", str);
        exit(1);
    }
}
/**
 * @brief The entry point to the program. Pulls the first and second argv, and generates
 * a btb table from the program's first argument (a trace file).
 */
int main(int argc, char *argv[]) {
    FILE *fp;
    enum state_machine_type state_machine = STATE_MACHINE_CLASS;
    struct btb_entry entries[BTB_SIZE] = { 0 };

    if (argc <= 2) {
        printf("Usage: %s <filename.txt> <A|B|CLASS>\n", argv[0]);
        return EXIT_SUCCESS;
    }

    state_machine = parse_state_machine_type(argv[2]);
    fp = fopen(argv[1], "r");
    
    if (fp == NULL) {
        fprintf(stderr, "Error opening file '%s': %s\n", argv[1], strerror(errno));
        exit(EXIT_FAILURE);
    }

    generate_btb(fp, entries, state_machine);
    // print_btb(entries, BTB_SIZE);
    
    fclose(fp);

    return EXIT_SUCCESS;
}