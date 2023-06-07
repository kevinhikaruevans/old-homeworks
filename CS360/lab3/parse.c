#include "parse.h"


/**
 * @brief Parse the env
 * 
 * @param env 
 */
void parse_env(char *env[], char *env_user, char *env_home, char **path_buffer) {
    int i = 0;

    while(env[i]) {
        if (STARTS_WITH(env[i], "USER=")) {
            DEBUG("$USER = %s", env[i] + 5);
            strcpy(env_user, env[i] + 5);
        }
        else if (STARTS_WITH(env[i], "HOME=")) {
            DEBUG("$HOME = %s", env[i] + 5);
            strcpy(env_home, env[i] + 5);
        }
        else if (STARTS_WITH(env[i], "PATH=")) {
            // if (strncmp(env[i], "PATH=", 5) == 0) {
            DEBUG("*** begin parsing PATH");

            strcpy(path_buffer, env[i] + 5);

            char* token = strtok(path_buffer, ":");
            char** dest = path;

            while(token) {
                DEBUG("   path = %s", token);
                *dest = token;
                token = strtok(NULL, ":");

                dest++;
            }
            *dest = NULL;
            DEBUG("*** end parsing PATH");
        }
        
        i++;
    }
}