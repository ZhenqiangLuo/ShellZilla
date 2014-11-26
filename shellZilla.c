#ifndef _SHELL_ZILLA_C_
#define _SHELL_ZILLA_C_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>

#define OK 0
#define ERROR -1

#define SHELL_HIS_CMD_CNT_MAX 10
#define PATH_LEN_MAX 1024
#define PROMPT_LEN_MAX 1024



/*command we now support*/
char *supCmd[] = 
{
    "currentdir",
    "ver",
    "ls",
    "cd",
    "mkdir",
    "rmdir",
    "commands",
    "history",
    "quit",
    NULL
};



int main()
{
    char shellPrompt[PROMPT_LEN_MAX];
    char cwd[PATH_LEN_MAX];
    char *userInput = NULL;
    char curFolder[64];
    char *inputWithNoSpace = NULL;
    char cmd[PROMPT_LEN_MAX];
    char param[PROMPT_LEN_MAX];

    /*init the completion function*/
    funShellZillaReadLineIni();

    /*set the command history count to 10*/
    stifle_history(SHELL_HIS_CMD_CNT_MAX);
    while(1)
    {
        memset(cwd, 0, sizeof(cwd));
        getcwd(cwd, sizeof(cwd));

        memset(curFolder, 0, sizeof(curFolder));
        /*just get the folder name in current directory*/
        funShellZillaGetCurFolder(cwd, curFolder);
        memset(shellPrompt, 0, sizeof(shellPrompt));
        
        sprintf(shellPrompt, "%s@%s:%s $", getenv("USER"), "ShellZilla", curFolder);

        /*read from stdinput*/
        userInput = readline(shellPrompt);
        if(NULL == userInput)
        {
            break;
        }

        /*get rid of the white space at the beginning and the end*/
        inputWithNoSpace = funShellZillaGetRidOfSpace(userInput);
        /*printf("%s\n", inputWithNoSpace);*/

        /*add the command to history*/
        add_history(inputWithNoSpace);
        if(!funShellZillaIsValidCmd(inputWithNoSpace))
        {
            /*if command not supported,print warning info*/
            printf("%s :command not found!\n", inputWithNoSpace);
            free(userInput);
            userInput = NULL;
            continue;
        }

        memset(cmd, 0, PROMPT_LEN_MAX);
        memset(param, 0, PROMPT_LEN_MAX);
        /*parse the input,get the command name and the parameter*/
        funShellZillaParse(inputWithNoSpace, cmd, param);
        if(!strcmp(cmd, "quit"))
        {
            free(userInput);
            userInput = NULL;
            break;
        }
        /*excute the command*/
        funShellZillaExc(cmd, param);

        free(userInput);
        userInput = NULL;
    }
    return OK;
}
