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


/**Variables para conocer el directorio actual de trabajo**/
char PWD[max_amb];
char SHELL[max_amb];
char PATH[max_amb];

/** Funciones declaradas para los comandos internos del shell, cd, help y exit**/
int my_cd(char **args);
int my_help(char **args);
int my_exit(char **args);

char *commands_list[] = {
  "cd", //comando paa cambiar de directorio
  "help", //Comando para obtener ayuda sobre el programa
  "exit" //Salir del programa
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
   @función Comando cd.
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
   @descripcion Comando help.
   @param No necesita argumentos.  
   @return retorna 1 y termina la ejecución.
 */

int my_help(char **args)
{
  int i;
  printf("\t Renzo Loor Minishell\n");
  printf("Escribe el nombre del programa y sus argumentos, y presiona enter.\n");
  printf("Los comandos internos son:\n");

  for (i = 0; i < num_commands(); i++) {
    printf("  %s\n", commands_list[i]);
  }

  printf("Usa el comando man para obtener información de otro programa.\n");
  return 1;
}

/**
   @descripcion Comando exit.
   @param No necesita argumentos.
   @return Retorna 0 y termina la ejecución.
 */
int my_exit(char **args)
{
  return 0;
}

/**
  @descripcion Lanza un programa y espera a que termine.
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
   @descripcion ejecuta el minishell o lanza un programa.
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
Lee las líneas ingresadas en la entrada estandar le asigna un espacio de memoria, si se excede se le reasigna una más grande.
 */
char *leer_linea(void)
{
  char *line = NULL;
  ssize_t bufsize = 0; 
  getline(&line, &bufsize, stdin); // Con la función getline se asigna un buffer
  return line;
}

#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"

/**
   dividir una línea en fragmentos en base a los espacios de línea 
 */
char **separador(char *line)
{
  int bufsize = LSH_TOK_BUFSIZE, position = 0; //Buffer donde se guardan las palabras
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token, **tokens_backup;

  if (!tokens) {
    fprintf(stderr, "error de asiganción\n");
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
        fprintf(stderr, "error de asignación\n");
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
void shell(void)
{
  char *linea;
  char **args;
  int estado;
  
  strcpy(PATH,getenv("PATH"));//Obtiene la ruta actual y la carga a PWD
  strcpy(SHELL,PWD);
  do {
    getcwd(PWD,max_amb); //Actualiza la ruta de trabajo
    printf("OSRenzoLoorShell$ %s> ", PWD);
    linea = leer_linea();
    args = separador(linea);
    estado = execute(args);
    
    free(linea);
    free(args);
  } while (estado); //Se ejecuta una vez antes de verificar el estado
}


int main(int argc, char **argv)
{
  // Carga la configuración
  clear();
  
  printf("\n \n \n*******************************************************");
  printf(" \n \t***miniShell**");
  printf("\n \n \t***Autor:    Renzo Loor***");
  printf("\n*******************************************************");
  sleep(2);
  clear();
  
  shell(); //ejecuta la función que simula el shell

  return EXIT_SUCCESS;
}

