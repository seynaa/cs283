#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "dshlib.h"

/*
 * Implement your exec_local_cmd_loop function by building a loop that prompts the 
 * user for input.  Use the SH_PROMPT constant from dshlib.h and then
 * use fgets to accept user input.
 * 
 *      while(1){
 *        printf("%s", SH_PROMPT);
 *        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
 *           printf("\n");
 *           break;
 *        }
 *        //remove the trailing \n from cmd_buff
 *        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
 * 
 *        //IMPLEMENT THE REST OF THE REQUIREMENTS
 *      }
 * 
 *   Also, use the constants in the dshlib.h in this code.  
 *      SH_CMD_MAX              maximum buffer size for user input
 *      EXIT_CMD                constant that terminates the dsh program
 *      SH_PROMPT               the shell prompt
 *      OK                      the command was parsed properly
 *      WARN_NO_CMDS            the user command was empty
 *      ERR_TOO_MANY_COMMANDS   too many pipes used
 *      ERR_MEMORY              dynamic memory management failure
 * 
 *   errors returned
 *      OK                     No error
 *      ERR_MEMORY             Dynamic memory management failure
 *      WARN_NO_CMDS           No commands parsed
 *      ERR_TOO_MANY_COMMANDS  too many pipes used
 *   
 *   console messages
 *      CMD_WARN_NO_CMD        print on WARN_NO_CMDS
 *      CMD_ERR_PIPE_LIMIT     print on ERR_TOO_MANY_COMMANDS
 *      CMD_ERR_EXECUTE        print on execution failure of external command
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 1+)
 *      malloc(), free(), strlen(), fgets(), strcspn(), printf()
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 2+)
 *      fork(), execvp(), exit(), chdir()
 */
int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff) {
    if (!cmd_line || strlen(cmd_line) == 0) {
        return WARN_NO_CMDS;
    }
    int argc = 0;
    char *token = cmd_line;
    bool in_quotes = false;

    while (*token && argc < CMD_ARGV_MAX - 1) {
        while (*token == ' ' && !in_quotes) {
            token++;
        }
        if (*token == '\0') {
            break;
        }
        char *start = token;
        if (*token == '\"') {
            in_quotes = true;
            start = ++token;  
        }
        while (*token) {
            if (*token == '\"') {
                in_quotes = !in_quotes; 
                *token = '\0'; 
                token++;
                break;
            } else if (!in_quotes && *token == ' ') {
                *token = '\0';
                token++;
                break; 
            } else {
                token++;
            }
        }
        cmd_buff->argv[argc++] = start;
    }

    cmd_buff->argv[argc] = NULL; 
    cmd_buff->argc = argc;

    return OK;
}

int exec_local_cmd_loop() {
    char cmd_buff[SH_CMD_MAX]; 
    cmd_buff_t cmd;             
    int rc = 0;

    if (alloc_cmd_buff(&cmd) != OK) {
        fprintf(stderr, "Error: Failed to allocate command buffer\n");
        return ERR_MEMORY;
    }
    while (1) {
        printf("%s", SH_PROMPT);
        if (fgets(cmd_buff, SH_CMD_MAX, stdin) == NULL) {
            printf("\n");
            break;
        }
        cmd_buff[strcspn(cmd_buff, "\n")] = '\0';
        if (strcmp(cmd_buff, EXIT_CMD) == 0) {
            break;
        }
        clear_cmd_buff(&cmd);
        rc = build_cmd_buff(cmd_buff, &cmd);
        if (rc == WARN_NO_CMDS) {
            printf("%s\n", CMD_WARN_NO_CMD);
            continue;
        } else if (rc == ERR_TOO_MANY_COMMANDS) {
            printf("%s\n", CMD_ERR_PIPE_LIMIT);
            continue;
        } else if (rc != OK) {
            fprintf(stderr, "Error: Failed to parse command\n");
            continue;
        }

        Built_In_Cmds bi_cmd = match_command(cmd.argv[0]);
        if (bi_cmd != BI_NOT_BI) {
            if (bi_cmd == BI_CMD_EXIT) {
                break;  
            } else if (bi_cmd == BI_CMD_CD) {
                exec_built_in_cmd(&cmd);
                continue;  
            }
            exec_built_in_cmd(&cmd);
            continue;  
        }
        pid_t pid = fork();
        if (pid == 0) {  
            if (execvp(cmd.argv[0], cmd.argv) == -1) {
                perror(CMD_ERR_EXECUTE);  
                exit(ERR_EXEC_CMD);
            }
        } else if (pid > 0) {  
            wait(NULL); 
        } else {
            perror("Fork failed");
        }
    }
    free_cmd_buff(&cmd);
    return OK;
}

Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd) {
    if (strcmp(cmd->argv[0], "cd") == 0) {
        if (cmd->argc == 1) {
            return BI_EXECUTED;  
        } else if (cmd->argc == 2) {
            if (chdir(cmd->argv[1]) != 0) {
                perror("cd");
                return BI_EXECUTED;
            }
        }
        return BI_EXECUTED;
    }
    return BI_NOT_BI;
}


Built_In_Cmds match_command(const char *cmd) {
    if (strcmp(cmd, "cd") == 0) {
        return BI_CMD_CD;
    } else if (strcmp(cmd, "exit") == 0) {
        return BI_CMD_EXIT;
    }
    return BI_NOT_BI;
}

int alloc_cmd_buff(cmd_buff_t *cmd) {
    cmd->_cmd_buffer = malloc(SH_CMD_MAX);
    if (!cmd->_cmd_buffer) {
        return ERR_MEMORY;
    }
    return OK;
}

int clear_cmd_buff(cmd_buff_t *cmd) {
    memset(cmd->_cmd_buffer, 0, SH_CMD_MAX);
    return OK;
}

int free_cmd_buff(cmd_buff_t *cmd) {
    free(cmd->_cmd_buffer);
    return OK;
}
