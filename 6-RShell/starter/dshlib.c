#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>

#include "dshlib.h"

int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff) {
    if (!cmd_line || strlen(cmd_line) == 0) {
        return WARN_NO_CMDS;
    }
    int argc = 0;
    char *token = cmd_line;
    bool in_quotes = false;

    cmd_buff->input_file = NULL;
    cmd_buff->output_file = NULL;
    cmd_buff->append_mode = false;

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
            } else if (*token == '<') {
                *token = '\0';
                token++;
                while (*token == ' ') token++;  
                cmd_buff->input_file = token;
                break;
            } else if (*token == '>') {
                *token = '\0';
                token++;
                while (*token == ' ') token++;  
                if (*token == '>') {
                    token++; 
                    cmd_buff->append_mode = true;
                }
                cmd_buff->output_file = token;
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

int build_cmd_list(char *cmd_line, command_list_t *clist) {
    char *cmd_str = strtok(cmd_line, PIPE_STRING);
    int cmd_count = 0;
    while (cmd_str != NULL && cmd_count < CMD_MAX) {
        int rc = build_cmd_buff(cmd_str, &clist->commands[cmd_count]);
        if (rc != OK) return rc;
        cmd_str = strtok(NULL, PIPE_STRING);
        cmd_count++;
    }
    clist->num = cmd_count;
    if (cmd_count >= CMD_MAX && strtok(NULL, PIPE_STRING) != NULL) {
        return ERR_TOO_MANY_COMMANDS;
    }
    return (cmd_count > 0) ? OK : WARN_NO_CMDS;
}

int execute_pipeline(command_list_t *clist) {
    int num_commands = clist->num;
    if (num_commands <= 0) return WARN_NO_CMDS;

    int pipefds[2 * (num_commands - 1)];
    pid_t pids[num_commands];

    for (int i = 0; i < num_commands - 1; i++) {
        if (pipe(pipefds + i * 2) == -1) {
            perror("pipe");
            return ERR_MEMORY;
        }
    }

    for (int i = 0; i < num_commands; i++) {
        pids[i] = fork();
        if (pids[i] < 0) return ERR_EXEC_CMD;

        if (pids[i] == 0) {
            if (i > 0) {
                if (dup2(pipefds[(i - 1) * 2], STDIN_FILENO) == -1) {
                    perror("dup2 stdin");
                    exit(ERR_EXEC_CMD);
                }
            }
            if (i < num_commands - 1) {
                if (dup2(pipefds[i * 2 + 1], STDOUT_FILENO) == -1) {
                    perror("dup2 stdout");
                    exit(ERR_EXEC_CMD);
                }
            }

            for (int j = 0; j < 2 * (num_commands - 1); j++) {
                close(pipefds[j]);
            }

            execvp(clist->commands[i].argv[0], clist->commands[i].argv);
            fprintf(stderr, "%s: %s\n", CMD_ERR_EXECUTE, strerror(errno));
            exit(ERR_EXEC_CMD);
        }
    }

    for (int i = 0; i < 2 * (num_commands - 1); i++) {
        close(pipefds[i]);
    }

    int status;
    int last_status = 0;
    for (int i = 0; i < num_commands; i++) {
        waitpid(pids[i], &status, 0);
        if (i == num_commands - 1) {
            last_status = WEXITSTATUS(status);
        }
    }

    return last_status;
}

int exec_local_cmd_loop() {
    char cmd_buff[SH_CMD_MAX]; 
    cmd_buff_t cmd;             
    int rc = 0;  
    int final_rc = OK;  

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

        command_list_t clist = {0};
        rc = build_cmd_list(cmd_buff, &clist);

        if (rc == WARN_NO_CMDS) {
            printf("%s\n", CMD_WARN_NO_CMD);
            continue;
        } else if (rc == ERR_TOO_MANY_COMMANDS) {
            printf(CMD_ERR_PIPE_LIMIT, CMD_MAX);
            continue;
        } else if (rc != OK) {
            fprintf(stderr, "Error: Failed to parse command\n");
            final_rc = rc;
            continue;
        }

        Built_In_Cmds bi_cmd = match_command(clist.commands[0].argv[0]);
        if (bi_cmd != BI_NOT_BI) {
            if (bi_cmd == BI_CMD_EXIT) {
                break;  
            }
            exec_built_in_cmd(&clist.commands[0]);
            free_cmd_list(&clist);
            continue;
        }

        rc = execute_pipeline(&clist);
        if (rc != OK && rc != ERR_EXEC_CMD) {
            fprintf(stderr, "Error executing pipeline\n");
            final_rc = rc;
        } else if (rc == ERR_EXEC_CMD) {
            final_rc = rc;  
        }
        free_cmd_list(&clist);
    }

    free_cmd_buff(&cmd);
    return final_rc;  
}

Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd) {
    Built_In_Cmds bi_cmd = match_command(cmd->argv[0]);
    if (bi_cmd == BI_NOT_BI) return BI_NOT_BI;

    switch (bi_cmd) {
        case BI_CMD_CD:
            if (cmd->argc == 1) {
                return BI_EXECUTED;
            } else if (cmd->argc == 2) {
                if (chdir(cmd->argv[1]) != 0) {
                    perror("cd");
                }
                return BI_EXECUTED;
            }
            break;
        case BI_CMD_EXIT:
            return BI_CMD_EXIT;
        case BI_CMD_STOP_SVR:
            return BI_CMD_STOP_SVR;
        case BI_CMD_DRAGON:
            printf("You are a brave soul, but I know not the command 'dragon'\n");
            return BI_NOT_IMPLEMENTED;
        case BI_CMD_RC:
            if (cmd->argc == 2) {
                int rc = atoi(cmd->argv[1]);
                exit(rc);
            }
            return BI_EXECUTED;
        default:
            return BI_NOT_BI;
    }
    return BI_EXECUTED;
}

Built_In_Cmds match_command(const char *cmd) {
    if (strcmp(cmd, "cd") == 0) return BI_CMD_CD;
    if (strcmp(cmd, "exit") == 0) return BI_CMD_EXIT;
    if (strcmp(cmd, "stop-server") == 0) return BI_CMD_STOP_SVR;
    if (strcmp(cmd, "dragon") == 0) return BI_CMD_DRAGON;
    if (strcmp(cmd, "rc") == 0) return BI_CMD_RC;
    return BI_NOT_BI;
}

int alloc_cmd_buff(cmd_buff_t *cmd) {
    cmd->_cmd_buffer = malloc(SH_CMD_MAX);
    if (!cmd->_cmd_buffer) {
        return ERR_MEMORY;
    }
    cmd->input_file = NULL;
    cmd->output_file = NULL;
    cmd->append_mode = false;
    for (int i = 0; i < CMD_ARGV_MAX; i++) cmd->argv[i] = NULL;
    cmd->argc = 0;
    return OK;
}

int clear_cmd_buff(cmd_buff_t *cmd) {
    if (cmd->_cmd_buffer) {
        memset(cmd->_cmd_buffer, 0, SH_CMD_MAX);
    }
    cmd->argc = 0;
    for (int i = 0; i < CMD_ARGV_MAX; i++) cmd->argv[i] = NULL;
    cmd->input_file = NULL;
    cmd->output_file = NULL;
    cmd->append_mode = false;
    return OK;
}

int free_cmd_buff(cmd_buff_t *cmd) {
    if (cmd->_cmd_buffer) free(cmd->_cmd_buffer);
    cmd->_cmd_buffer = NULL;
    return OK;
}

int free_cmd_list(command_list_t *cmd_lst) {
    for (int i = 0; i < cmd_lst->num; i++) {
        clear_cmd_buff(&cmd_lst->commands[i]);
    }
    cmd_lst->num = 0;
    return OK;
}