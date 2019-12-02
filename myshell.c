/***************************************************************************//**

  @file         myshell.c

  @author       Renzo Loor

  @date         Sunday,  1 December 2019

*******************************************************************************/
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define max_amb 1000 //Número máximo de carateres para las variables de ambiente
#define clear() printf("\033[H\033[J")

char PWD[max_amb];
char SHELL[max_amb];
char PATH[max_amb];

/** Funciones declaradas para los comandos del shell**/
int my_cd(char **args);
int my_help(char **args);
int my_exit(char **args);

char *commands_list[] = {
  "cd",
  "help",
  "exit"
};

int (*commands_func[]) (char **) = {
  &my_cd,
  &my_help,
  &my_exit
};

int num_commands() { //Número de comando internos
  return sizeof(commands_list) / sizeof(char *);
}

/** Implementación de los comandos internos**/

/**
   @brief Comando cd.
   @param Argumentos.  args[0] es "cd".  args[1] es el directorio.
   @return retorna 1 y termina la ejecución.
 */

int my_cd(char **args)
{
  if (args[1] == NULL) { //Si no se pasan argumentos
    fprintf(stderr, "error: no has pasado argumentos al comando \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("lsh");
    }
  }
  return 1;
}

/**
   @brief Comando help.
   @param No necesita argumentos.  
   @return retorna 1 y termina la ejecución.
 */

int my_help(char **args)
{
  int i;
  printf("\t Renzo Loor Minishell\n");
  printf("Escribe el nombre del progrema y sus argumentos, y presiona enter.\n");
  printf("Los comandos internos son:\n");

  for (i = 0; i < num_commands(); i++) {
    printf("  %s\n", commands_list[i]);
  }

  printf("Usa el comando man para obtener información de otro programa.\n");
  return 1;
}

/**
   @brief Comando exit.
   @param No necesita argumentos.
   @return Retorna 0 y termina la ejecución.
 */
int my_exit(char **args)
{
  return 0;
}

/**
  @brief Lanza un programa y espera a que termine.
   @return Retorna 1 para continuar la ejecución.
 */
int launch(char **args)
{
  pid_t pid;
  int status;

  pid = fork(); //Se crea un proceso
  if (pid == 0) {//Ejecución del proceso hijo
    if (execvp(args[0], args) == -1) {
      perror("error");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {// en caso de error
    perror("error");
  } else {// ejecución del proceso padre
    do {
      waitpid(pid, &status, WUNTRACED);//Espera hasta que el proceso hijo haya terminado
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}

/**
   @brief ejecuta el minishell o lanza un programa.
   @return 1 si el minishell continua ejecutándose, 0 si terminó
 */
int execute(char **args)
{
  int i;

  if (args[0] == NULL) {// Si no se ha escrito ningún comando.
    return 1;
  }

  for (i = 0; i < num_commands(); i++) {
    if (strcmp(args[0], commands_list[i]) == 0) {
      return (*commands_func[i])(args);
    }
  }

  return launch(args);
}

#define BUFFER_SIZE 1024

/**
Lee las líneas ingresadas en la entrada estandar.
 */
char *read_line(void)
{
  int bufsize = BUFFER_SIZE;
  int position = 0;
  char *buffer = malloc(sizeof(char) * bufsize);
  int c;

  if (!buffer) {
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  while (1) {
    // Read a character
    c = getchar();

    if (c == EOF) {
      exit(EXIT_SUCCESS);
    } else if (c == '\n') {
      buffer[position] = '\0';
      return buffer;
    } else {
      buffer[position] = c;
    }
    position++;

    // If we have exceeded the buffer, reallocate.
    if (position >= bufsize) {
      bufsize += BUFFER_SIZE;
      buffer = realloc(buffer, bufsize);
      if (!buffer) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }
  }
}

#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"

/**
   dividir una línea en tokens
 */
char **split_line(char *line)
{
  int bufsize = LSH_TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token, **tokens_backup;

  if (!tokens) {
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, LSH_TOK_DELIM);
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += LSH_TOK_BUFSIZE;
      tokens_backup = tokens;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) {
		free(tokens_backup);
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, LSH_TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}

/**
   Lazo entre la entrada y la ejecución.
 */
void loop(void)
{
  char *line;
  char **args;
  int status;
  //getcwd(PWD,max_amb); //Obtiene la ruta actual y la carga a PWD
  strcpy(PATH,getenv("PATH"));
  strcpy(SHELL,PWD);
  do {
    getcwd(PWD,max_amb);
    printf("OSRenzoLoorShell %s> ", PWD);
    line = read_line();
    args = split_line(line);
    status = execute(args);
    
    free(line);
    free(args);
  } while (status);
}

/**
   @brief Main entry point.
   @param argc Argument count.
   @param argv Argument vector.
   @return status code
 */
int main(int argc, char **argv)
{
  // Load config files, if any.
  clear();
  
  printf("\n \n \n \t *******************************************************");
  printf(" \n \t***miniShell**");
  printf("\n \n \t***Autor:    Renzo Loor***");
  printf("\n \n \n \t*******************************************************");
  sleep(2);
  clear();
  // Run command loop.
  loop();

  // Perform any shutdown/cleanup.

  return EXIT_SUCCESS;
}

