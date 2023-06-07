#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>

#include "./partition.h"

void print_partition(const char *file, int fd, int offset_sector, int index, struct partition *p);
int read_partitions(int fd, struct partition *dest, int count);
void print_ext_partitions(const char *file, int fd,
                          int master_offset_sector, int index, struct partition *p);
void print_partitions(const char *file);

/**
 * @brief Lil helper function to read a partition table
 */
int read_partitions(int fd, struct partition *dest, int count)
{
    return (int)read(fd, dest, sizeof(struct partition) * count);
}

/**
 * @brief Prints out the full partition table for an extended partition
 */
void print_ext_partitions(const char *file, int fd,
                          int master_offset_sector, int index, struct partition *p)
{
    struct partition partitions[2];
    int start = master_offset_sector + p->start_sector;

    lseek(fd, start * 512 + MBR_TABLE_OFFSET, SEEK_SET);
    read_partitions(fd, partitions, 2);

    // the first partition is offset from the master offset and can be printed immediately
    print_partition(file, fd, master_offset_sector + p->start_sector, index + 1, partitions);

    // if there's a second partition, recurse into that
    if ((partitions + 1)->start_sector > 0)
    {
        if (master_offset_sector == 0)
        {
            master_offset_sector = p->start_sector;
        }

        print_ext_partitions(
            file,
            fd,
            master_offset_sector,
            index + 1,
            partitions + 1);
    }
}

/**
 * @brief Prints a formatted partition for a partition table.
 * 
 * @param file The filename
 * @param fd The file descriptor
 * @param offset_sector The base sector offset
 * @param index The zero-based index of the partition
 * @param p The partition to print a row for
 */
void print_partition(const char *file, int fd, int offset_sector, int index, struct partition *p)
{
    printf(
        "%s%d%6d%6d%8d%6dK %2x %s\n",
        file, (index + 1),
        offset_sector + p->start_sector,
        offset_sector + p->start_sector + p->nr_sectors - 1,
        p->nr_sectors,
        (p->nr_sectors * 512) / 1024,
        p->sys_type,
        get_strtype(p->sys_type));

    if (offset_sector == 0 && p->sys_type == PARTITION_TYPE_EXT)
    {
        print_ext_partitions(file, fd, 0, index, p);
    }
}

/**
 * Prints the partitions of a virtual disk file
 * @param file The filename of the virtual disk
 */
void print_partitions(const char *file)
{
    int fd = open(file, O_RDONLY);
    struct partition partitions[MBR_PARTITION_COUNT];

    lseek(fd, MBR_TABLE_OFFSET, SEEK_SET);
    read_partitions(fd, &partitions[0], MBR_PARTITION_COUNT);

    printf("Device Start   End Sectors   Size Id Type\n");

    /* print the first four partitions and recurse, if needed */
    for (int i = 0; i < MBR_PARTITION_COUNT; i++)
    {
        if (!(partitions + i)->start_sector)
            break;

        print_partition(
            file,
            fd,
            0,
            i,
            partitions + i);
    }

    close(fd);
}

int main()
{
    print_partitions("vdisk");

    return 0;
}
