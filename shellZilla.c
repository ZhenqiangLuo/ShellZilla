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


/*history array*/
typedef struct _stShellHistory_
{
    int cnt;/*history count max 10*/
    char *historyArr[SHELL_HIS_CMD_CNT_MAX]; 
}stShellHistory;
stShellHistory history;



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


/*command explanation*/
char *supCmdExplain[] = 
{
    "show current working directory.",
    "show the version, feature description, author and so on.",
    "built-in command ls in bash.",
    "change diretory.",
    "built-in command mkdir in bash.",
    "built-in command rmdir in bash.",
    "show all the commands that I support.",
    "show the history commands",
    "quit me.",
    NULL
};


/*for command tab completion*/
char *funShellZillaCmdGen(const char *text, int state)
{
    const char *name;
    static int list_index, len;

    if (!state)
    {
        list_index = 0;
        len = strlen (text);
    }

    while ((supCmd[list_index] != NULL) && (name = supCmd[list_index]))
    {
        list_index++;
        if (strncmp (name, text, len) == 0)
        {
            return strdup(name);
        }
    }

    return ((char *)NULL);   
}

char** funShellZillaCmdCompletion(const char* text, int start, int end)
{
    char **matches;  
  
    matches = (char **) NULL;  
    if (start == 0)  
    {
        matches = rl_completion_matches(text, funShellZillaCmdGen); 
    }
  
    return matches;
}

void funShellZillaReadLineIni()
{
    rl_readline_name = "ShellZilla";
    rl_attempted_completion_function = funShellZillaCmdCompletion;
    return;
}


/*get current folder name*/
void funShellZillaGetCurFolder(char *pCwd, char *curFolder)
{
    char *p = NULL;
    int len = 0;
    
    p = pCwd + strlen(pCwd);
    /*find the last / from the directory*/
    while(*p != '/')
    {
        len++;
        p--;
    }
    memcpy(curFolder, p+1, len);
    return;
}


char *funShellZillaGetRidOfSpace(char *input)
{
    char *p = NULL;
    char *q = NULL;

    p = input;
    /*find the first char that is not whitespace*/
    while(*p == ' ')
    {
        p++;
    }

    /*get rid of the white space at the end*/
    q = input+strlen(input)-1;
    while(*q == ' ')
    {
        q--;
    }
    q++;
    *q = '\0';
    return p;
}

/*function for command ver*/
void funShellZillaExcVer(char **argv)
{
    printf("\r\n**********************************************************************\n");
    printf("**************************ShellZilla**********************************\n");
    printf("**********************************************************************\n");
    printf("*current version    : 2.1                                            *\n");
    printf("*feature description: 1.recording 10 history commands                *\n");
    printf("*                     2.tab completion for commands                  *\n");
    printf("*                     3.8 command type supported                     *\n");
    printf("*                     4.recording the previous command               *\n");
    printf("*                     5.indicating the authors                       *\n");
    printf("*                       and the version of this Shell                *\n");
    printf("*                     6.listing all supported commands 				 *\n");
    printf("*                     7.allowing user the exit the Shell             *\n");
    printf("*                     8.fixed some bugs                              *\n");
    printf("*author             : Tianqi Zhou, Wentao Xu                         *\n");
    printf("*                     Yuming Zhang, Zhenqiang Luo                    *\n");
    printf("**********************************************************************\r\n");
    return;
}




/*if the command input not in supCmd, it is invalid,else valid*/
int funShellZillaIsValidCmd(char *input)
{
    char *p = NULL;
    int index = 0;
    int flag = 0;
    
    while(supCmd[index] != NULL)
    {
        p = supCmd[index];
        if(strncmp(p, input, strlen(p)) == 0)
        {
            flag = 1;
            break;
        }
        index++;
    }
    return flag;
}



/*according to input,get the command name and parameter*/
void funShellZillaParse(char *input, char **argv)
{
    char *p = NULL;

    p = input;
    while (*p != '\0')
    {
        while ((*p == ' ') 
            || (*p == '\t') 
            || (*p == '\n')) 
        {
            /*if white space,tab,or newline encountered，one command finish*/
            *p = '\0';
            p++;
        }
        // save the position of argument
        *argv = p;
        argv++;
        while((*p != '\0' )
                && (*p != ' ') 
                && (*p != '\t') 
                && (*p != '\n'))
        {
            p++;
        }
    }
    *argv = (char *) '\0';    
    return;
}

/*get current directory,the same as pwd command in bash*/
void funShellZillaExcCurDir(char **argv)
{
    char cwd[PATH_LEN_MAX];
    memset(cwd, 0, sizeof(cwd));
    getcwd(cwd, sizeof(cwd));
    printf("Your current working directory is: \n");
    printf("%s\n", cwd);
    return;
}

/*funtion for command commands*/
void funShellZillaExcCmd(char **argv)
{
    int index = 0;
    char *p = NULL;

    printf("\r\nThe following commands supported:\n\n");
    while(supCmd[index] != NULL)
    {
        printf("%s: %s\n", supCmd[index], supCmdExplain[index]);
        index++;
    }
    printf("\r\n");
    return;
}

void funShellZillaExcHis(char **argv)
{
    char *pHistory = NULL;
    int index = 1;

    for(index=0; index<history.cnt; index++)
    {
        pHistory = history.historyArr[index];
        if(NULL == pHistory)
        {
            continue;
        }
        printf("%s\n", pHistory);
    }
        
    return;    
}

void funShellZillaExcBuiltInBash(char **argv)
{
    pid_t pid;
    pid = fork();
    if(pid < 0)
    {
        printf("fork child process failed\n");
        return;
    }
    else if (pid == 0)
    {
        if (execvp(*argv, argv) < 0)
        {
            printf("exec cmd failed\n");
            return;
        }
    }
    else
    {
        int wc = wait(NULL);
        if (wc == ERROR)
        {
            printf("wait child failed\n");
            return;
        }
    }
    return;
}


/*uniq funtion for command excution*/
void funShellZillaExc(char **argv)
{
    char cmdStr[1024];
    if(!strcmp(*argv, "currentdir"))
    {
        funShellZillaExcCurDir(argv);
    }

    if(!strcmp(*argv, "ver"))
    {
        funShellZillaExcVer(argv);
    }

    if(!strcmp(*argv, "commands"))
    {
        funShellZillaExcCmd(argv);
    }

    if(!strcmp(*argv, "history"))
    {
        funShellZillaExcHis(argv);
    }

    if(!strcmp(*argv, "ls") 
        || !strcmp(*argv, "mkdir")
        || !strcmp(*argv, "rmdir"))
    {
        /*built-in function in bash*/
        funShellZillaExcBuiltInBash(argv);
    }

    if(!strcmp(*argv, "cd"))
    {
        chdir(argv[1]);
    }
    return;
}

void funShellZillaPrintWelCome()
{
	printf("\n\n\n");
    printf("\r\n**********************************************************************\n");
    printf("***********Hi! My name is ShellZilla, your personal shell*************\n");
    printf("***********(Enter commands to see what I can do for you)**************\n");
    printf("**********************************************************************\n");
    printf("How can I help you? \n\n\n");
    return;
}

void funShellZillaAddHistoryList(char *input)
{
    char *pHistory = NULL;
    char *p = NULL;
    int i = 0;

    pHistory = (char *)malloc(strlen(input)+1);
    if(NULL == pHistory)
    {
        return;
    }
    memset(pHistory, 0, strlen(input)+1);
    strcpy(pHistory, input);
    if(history.cnt < SHELL_HIS_CMD_CNT_MAX)
    {
        /*find a place and insert it*/
        for(i=0; i<SHELL_HIS_CMD_CNT_MAX; i++)
        {
            if(history.historyArr[i] == NULL)
            {
                break;
            }
        }
        history.historyArr[i] = pHistory;
        history.cnt++;
    }
    else
    {
        /*delete the first record*/
        /*record the first history command*/
        p = history.historyArr[0];
        for(i=1; i<SHELL_HIS_CMD_CNT_MAX; i++)
        {
            /*left move the history*/
            history.historyArr[i-1] = history.historyArr[i];
        }
        history.historyArr[SHELL_HIS_CMD_CNT_MAX-1] = pHistory;
        free(p);
        p = NULL;
    }
    return;
}

void funShellZillaHistoryListFree()
{
    int i = 0;
    char *p = NULL;
    for(i=0; i<history.cnt; i++)
    {
        p = history.historyArr[i];
        if(NULL != p)
        {
            free(p);
            p = NULL;
        }
    }
    memset(&history, 0, sizeof(history));
    return;
}


int main()
{
    char shellPrompt[PROMPT_LEN_MAX];
    char cwd[PATH_LEN_MAX];
    char *userInput = NULL;
    char curFolder[64];
    char *inputWithNoSpace = NULL;
    char *argv[64];

    memset(&history, 0, sizeof(history));
    /*init the completion function*/
    funShellZillaReadLineIni();
    /*set the command history count to 10*/
    stifle_history(SHELL_HIS_CMD_CNT_MAX);
    funShellZillaPrintWelCome();
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
        funShellZillaAddHistoryList(inputWithNoSpace);
        if(!funShellZillaIsValidCmd(inputWithNoSpace))
        {
            /*if command not supported,print warning info*/
            printf("%s :command not found!\n", inputWithNoSpace);
            free(userInput);
            userInput = NULL;
            continue;
        }

        /*parse the input,get the command name and the parameter*/
        funShellZillaParse(inputWithNoSpace, argv);
        if(!strcmp(*argv, "quit"))
        {
            free(userInput);
            userInput = NULL;
            break;
        }
        /*excute the command*/
        funShellZillaExc(argv);

        free(userInput);
        userInput = NULL;
    }
    funShellZillaHistoryListFree();
    return OK;
}
#endif