#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <math.h>

// Constants
#define BUFFER 256
#define ADDSIZE 64
#define ERROR "Error! Program Terminated\n"

struct Shell
{
	char *currentDirectory;
	char **history;
	int exit;
};

// Function to run shell
void runShell(struct Shell shell);
char *readLine();
char **parser(char *command);
struct Shell output(char **parsedInput, struct Shell shell);
struct Shell start(struct Shell shell, char **parsedInput);


// Helper functions
int exhistingDirectory(char *directory);
char **clearStringArray(char **strArr);

// F_OK - test if a file exhists

int main()
{

	printf("Welcome to the shell:\n\n");

	char buffer[BUFFER];
	struct Shell shell;
	shell.currentDirectory = getcwd(buffer, BUFFER);
	shell.history = malloc(sizeof(char) * BUFFER * BUFFER);
	shell.exit = 1;
	runShell(shell);

	for (int i = 0; shell.history[i]; i++)
		free(shell.history[i]);

	free(shell.history);
	return 0;

}

void runShell(struct Shell shell)
{
	char *input = malloc(sizeof(char) * BUFFER);
	char **parsedInput = malloc(sizeof(char) * BUFFER);
	int i = 0;

	do
	{
		printf("# ");
		input = readLine();
		if (!(strcmp(input, "history") == 0))
		{
			shell.history[i] = malloc(sizeof(char) * BUFFER);
			strcpy(shell.history[i], input);
			i++;
		}

		if (strcmp(input, "history -c") == 0)
			i = 0;

		parsedInput = parser(input);	

		shell = output(parsedInput, shell);

	}
	while(shell.exit);

	free(input);
	free(parsedInput);

	printf("exiting.\n");
}

char *readLine()
{
	int size = BUFFER;
	int i = 0;
	char *buffer = malloc(sizeof(char) * size);
	int c;

	if (!buffer)
	{
		printf("%s", ERROR);
		exit(EXIT_FAILURE);
	}

	while(1)
	{
		c = getchar();

		if (c == '\n')
		{
			buffer[i] = '\0';
			return buffer;
		}
		else
		{
			buffer[i] = c;
		}
		i++;

		if (i >= size)
		{
			printf("%s", ERROR);
			exit(EXIT_FAILURE);
		}
	}
}

char **parser(char *command)
{
	int size = BUFFER;
	int i = 0;
	char **tokens = malloc(sizeof(char*) * size);
	char *token;

	if (!tokens)
	{
		printf("%s", ERROR);
		exit(EXIT_FAILURE);
	}

	token = strtok(command, " \t\r\n\a");

	while(token != NULL)
	{
		tokens[i] = token;
		i++;

		if (i >= size)
		{
			size += ADDSIZE;
			tokens = realloc(tokens, sizeof(char*) * size);

			if (!tokens)
			{
				printf("%s", ERROR);
				exit(EXIT_FAILURE);
			}
		}

		token = strtok(NULL, " \t\r\n\a");
	}

	tokens[i] = NULL;
	return tokens;

}

struct Shell output(char **parsedInput, struct Shell shell)
{
	if (strcmp(parsedInput[0], "movetodir") == 0)
	{
		if (exhistingDirectory(parsedInput[1]))
		{
			shell.currentDirectory = parsedInput[1];
			printf("\n%s\n", shell.currentDirectory);
			return shell;
		}

		printf("Error: %s is not an exhisting Directory\n", parsedInput[1]);
		return shell;
	}

	else if (strcmp(parsedInput[0], "whereami") == 0)
	{
		printf("%s\n", shell.currentDirectory);
		shell.history = realloc(shell.history, sizeof(char) * BUFFER);
		return shell;
	}

	else if (strcmp(parsedInput[0], "history") == 0)
	{
		if (parsedInput[1])
			if (strcmp(parsedInput[1], "-c") == 0)
			{
				shell.history = clearStringArray(shell.history);
				return shell;
			}

		printf("\nHISTORY:\n");
		for (int i = 0; shell.history[i]; i++)
		{
			printf("\t%s\n", shell.history[i]);
		}
		return shell;
	}

	else if (strcmp(parsedInput[0], "byebye") == 0)
	{
		shell.exit = 0;
		return shell;
	}

	else if (strcmp(parsedInput[0], "start") == 0)
	{
		shell = start(shell, parsedInput);
		return shell;
	}

	else if (strcmp(parsedInput[0], "background") == 0)
	{
		shell = start(shell, parsedInput);
		return shell;
	}

	else if (strcmp(parsedInput[0], "exterminate") == 0)
	{
		pid_t victim = atoi(parsedInput[1]);
		int isKill = kill(victim, SIGKILL);

		if (isKill == -1)
		{
			printf("ERROR: failed to kill process %d.\n", victim);
			return shell;
		}

		return shell;
	}

	printf("Command %s Not Recognised\n", parsedInput[0]);
	return shell;

}

int exhistingDirectory(char *directory)
{
	int result = access(directory, F_OK);
	if (result == 0)
		return 1;
	 else
		 return 0;
}

char **clearStringArray(char **strArr)
{
	for (int i = 0; strArr[i]; i++)
	{
		for (int j = 0; strArr[i][j]; j++)
		{
			strArr[i][j] = '\0';
		}
		strArr[i] = NULL;
	}

	return strArr;
}

struct Shell start(struct Shell shell, char **parsedInput)
{
	int childStatus;
	char *path = malloc(sizeof(char) * BUFFER);

	if (parsedInput[1][0] == '/')
	{
		strcat(path, parsedInput[1]);
	}
	else
	{
		strcat(path, shell.currentDirectory);
		strcat(path, "/");
		strcat(path, parsedInput[1]);
	}

	if (!exhistingDirectory(path))
	{
		printf("ERROR: File does not exist\n");
		return shell;
	}

	pid_t pid = fork();

	if (pid == 0)
	{
		char **args = malloc(sizeof(char) * BUFFER);
		args[0] = path;
		for (int i = 2, j = 1; parsedInput[i]; i++, j++)
		{
			args[j] = parsedInput[i];
		}
		printf("Child Process %d\n", getpid());
		execv(args[0], args);
	}

	else if (pid < 0)
	{
		printf("ERROR: Fork process failed!\n");
		return shell;
	}

	else
	{
		if (strcmp(parsedInput[0], "background") == 0)
		{
			printf("The PID for this process is: %d\n", getpid());
		}
		printf("Parent Process %d\n", getpid());
		waitpid(pid, &childStatus, WUNTRACED);
	}

	free(path);
	return shell;
}

