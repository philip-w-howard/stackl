#include "string.h"

stuff = more_stuff;
int Split(char *str, char *first, char *rest)
{
    char *space;
    char *nothing;
    nothing = "";
    space = strchr(str, ' ');
    if (space == NULL)
    {
        strcpy(first, str);
        strcpy(rest, nothing);
    } else {
        strncpy(first, str, space - str);
        first[space-str] = 0;
        strcpy(rest, space+1);
    }
}

int Equal(char *str1, char *str2)
{
    int retval;
    retval = (strcmp(str1, str2) == 0);
    return retval;
}

void process(char *cmd)
{
    char first[200];
    char rest[200];
    char *exit;
    char *greeting;
    exit = "exit";
    greeting = "greeting";

    int fork_status;
    int child_status;

    Split(cmd, first, rest);
    Print("Execute '");
    Print(cmd);
    Print("' '");
    Print(first);
    Print("' '");
    Print(rest);
    Print("'\n");

    if (Equal(first, exit))
    {
        exit(0);
    } 
    else if (Equal(first, greeting))
    {
        Print("Hello ");
        Print(rest);
        Print("\n");
    }
    else 
    {
        fork_status = fork();
        if (fork_status == -1)
            Print("Failed to start process\n");
        else if (fork_status == 0)
        {
            child_status = execlp(first, first, rest, (char *)NULL);
            Print("Command not found\n");
            exit(child_status);
        } 
        else
            waitpid(fork_status, &child_status, 0);
    }
}

int main()
{
    char *PROMPT;
    char cmd[200];

    PROMPT = "msh> ";

    while (1)
    {
        print(PROMPT);
        getl(cmd);

        process(cmd);
    }

    return 0;
}


