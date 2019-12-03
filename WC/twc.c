/***************************************************************************//**

  @file         twc.c

  @author       Renzo Loor

  @date         Sunday,  1 December 2019

*******************************************************************************/
#include <pthread.h>
#include <stdio.h>
#include <ctype.h>

typedef unsigned long count_t;
typedef enum {false=0, true=1} bool;

void *thread_job(void *);
void *aux_thread_job(void *);

/**
   @descripción Variables para contadores de palabras, líneas y caracteres.
   @param Sin argumentos.  
 */

count_t caracter_total; //Contador de caracteres totales
count_t palabras_total;//Contador de palabras totales
count_t lineas_total;//Contador de líneas totales

/*Contadores para más de un archivo*/
count_t caracter_final;
count_t palabras_final;
count_t lineas_final;


int nthreads; //Número de hilos con el cual trabajar


struct thread_data_structure{//estructura con los contadores y un buffer de 30
   int  thread_id;
   count_t caracter;
   count_t palabras;
   count_t lineas;
   count_t error;
   char buffer[30];
};

/**
   @función contador.
   @param Argumentos.  args[0] es el comando twc.  args[1] es el directorio de el/los archivo/s.
   @return retorna 1 y termina la ejecución, mostrando en pantalla los contadores.
 */

void contador(int argc, char **argv){ //Función que hará el cálculo por cada hilo

	printf("Enter the number of threads: ");
  	scanf("%d",&nthreads); // El número de hilos será dado por el usuario

	struct thread_data_structure thread_data[nthreads];
	FILE *f; //Inicialización de una variable archivo
	int m=1;
	pthread_t thread[nthreads],excess_thread;

	if (argc < 2) printf ("Modo de uso: twc ruta_archivo1 ruta_archivo2 .. [../file]\n"); //Si hay menos de dos argumentos sale un mensaje del modo de uso

	for (; m < argc; m++)
	{
	  if (f = fopen(argv[m], "rt")) //Si el archivo se  abre sin problemas
	  {		 	
				/*Seteando los valores iniciales a cero, por cada iteración se aumentan las variables dependiendo si es caracter, linea o palabra*/
				count_t caracter_total=0;
				count_t palabras_total=0;
				count_t lineas_total=0;

				int i,excess=0;
				while (!feof(f)) //Mientras no sea el final del archivo
				{
					bool flag=false; //bandera
					i=0;
					int round=0;
					for(flag=false; i<nthreads;i++,round++)
					{
						excess=(fread(thread_data[i].buffer, 1, 30, f));
						thread_data[i].thread_id=i;
						if(excess==30){
							pthread_create(&thread[i], NULL, thread_job, (void *) &thread_data[i]); //Crear hilos
						}		
	 					else{	//Función de invocación que calcula en serie los argumentos restantes: buffer [i] y exceso
	
							thread_data[i].error=excess;
							pthread_create(&excess_thread,NULL, aux_thread_job, (void*) &thread_data[i]);
							flag=true;
							break;
						}
					}// Unir hilos al hilo principal y realizar la operación de adición
	
					for(i=0; i<round;i++)
					{
						if(pthread_join(thread[i], NULL)){
						fprintf(stderr, "Error al unir los hilos[%d] \n",i);}
						else{
							caracter_total+=thread_data[i].caracter;
							palabras_total+=thread_data[i].palabras;
							lineas_total+=thread_data[i].lineas;
						
	       					}
					}
						if(flag){	/*Uniendo el último hilo*/
					  			if(pthread_join(excess_thread, NULL)){
									fprintf(stderr, "Error al unir los hilos excedentes\n");
									}
								else{        
									caracter_total+=thread_data[i].caracter;
									palabras_total+=thread_data[i].palabras;
									lineas_total+=thread_data[i].lineas;


										}
								}
				}


	printf("\n\n\t\n*******************************************\n\t*NOMBRE DE ARCHIVO: %s*\n*******************************************\n\tCaracteres totales: %6lu \n\tNúmero de palabras: %6lu \n\tNúmero de líneas:   %6lu \n\n",argv[m],caracter_total,palabras_total,lineas_total);
	fclose(f);

	
	caracter_final+=caracter_total;//Caracteres de todos los archivos
	palabras_final+=palabras_total;//Palabras de todos los arhivos
	lineas_final+=lineas_total;//lineas totales de todos los archivos

	}

	else
	{
		printf("No se pudo abrir el archivo o no existe %s \n", argv[m]);
	}

	}

	if(m>2)
	{
			printf("\n\n\t\n*******************************************\n\t*RESULTADO TOTAL*\n*******************************************\n\tCaracteres totales: %6lu \n\tNúmero de palabras: %6lu \n\tNúmero de líneas:   %6lu \n\n",caracter_final,palabras_final,lineas_final);


	}
	return;
} //fin contador

int main(int argc, char **argv)
{
	
  contador(argc,argv);
}//main

/**
   @función thread_job.
   @param Parámetros: void.
   @descripcion Función para subprocesos: USO: Procese la gran cantidad de datos de 30 caracteres y complete Ccount, Wcount y Lcount.
 */
void *thread_job(void *threadarg)
{

   struct thread_data_structure *my_data;
   my_data = (struct thread_data_structure *) threadarg;
	my_data->caracter=0;
	my_data->palabras=0;
	my_data->lineas=0;
	my_data->error=0;
   int k=0;
		   		while(k<30)
		{
				while(k<30)
				{
					if(isalpha(my_data->buffer[k])){
					my_data->palabras++;
					break;}
					my_data->caracter++;
					 if ((my_data->buffer[k]) == '\n')
             my_data->lineas++;
				k++;
				}
				while(k<30)
				{
					my_data->caracter++;
			if ((my_data->buffer[k]) == '\n')
             my_data->lineas++;
			if (!isalpha(my_data->buffer[k++]))
             break;
				}
		}				
}		

/**
   @función aux_thread_job.
   @param Argumentos: void.
   @descripcion Función para caracteres en exceso: USO: procese los caracteres en exceso y complete el Ccount, Wcount y Lcount.
 */
void *aux_thread_job(void *threadarg)
{
   struct thread_data_structure *my_data;
   my_data = (struct thread_data_structure *) threadarg;
   int excess= my_data->error;
	my_data->caracter=0;
	my_data->palabras=0;
	my_data->lineas=0;
	my_data->error=0;
   int k=0;


	while(k<excess)
		{
				while(k<excess)
				{
					if(isalpha(my_data->buffer[k])){
					my_data->palabras++;
					break;}
					my_data->caracter++;
					 if ((my_data->buffer[k]) == '\n')
             my_data->lineas++;
				k++;
				}
				while(k<excess)
				{
					my_data->caracter++;
			if ((my_data->buffer[k]) == '\n')
             my_data->lineas++;
			if (!isalpha(my_data->buffer[k++]))
             break;
			
				}
		}

}

