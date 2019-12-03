# Proyecto de Sistemas Operativos
Nombre: Renzo Loor Porro
# Descripción
Implementación de un mini shell con tres funciones internas:
CD: Cambia el directorio, el cual usa el comando chdir.
Help: Una breve explicación de la funcionalidad del programa y como ejecutarlo.
Exit: Termina la ejecución del programa

# Diseño:
El programa puede ejecutar funciones internas como cd, help y exit. Sin embargo también puede ejecutar programas externos, nativos del sistema operativo Linux como nano, cat, touch, etc.
Para ejecutar una función externa, en la función "launch", se crea un proceso con el comando fork, el proceso hijo se ejecuta mientras el padre espera a que termine con el comando "waitpid". 
De esta forma se asegura que el proceso hijo se termine de ejecutar y se termine el proceso padre con ejecuciones pendientes.

# Especificaciones
Para este proyecto se analizó toda la linea escrita por el usuario, hasta un límite de 1000 caracteres para las variables de ambiente, Se separaron las palabras dependiendo de los espacion, los cuales era ignorados. Esto permitia que se escriban dobles espacios o tabs y que no afecte el funcionamiento del sistema. 
La primera palabra representaba el comando a ejecutarse y las siguientes palabras represetaban los parametros.
Se asignó un espacio de memoria en la función leer_linea con el comando getline, si los caracteres escritos sobrepasa el buffer, esta función reasignaba más memoria automaticamente.

# Problemas
Para este proyecto se ha implementado la función exit de tal forma que termina la ejecución del programa inmediatamente despues de escribir el comando, sin verificar si algún proceso en segundo plano esta trabajando. 

# Descripción
Implementación de un contador de palabras optimizado para trabajar con hilos, el programa lee uno o varios archivos las siguientes características:
Número total de palabras
Número total de líneas
Número total de caracteres

# Diseño
El programa pide por entrada de teclado la cantidad de hilos a utilizar, dependiendo de esa cantidad divide el archivo por cada hilo y hace el procedimiento, al final los resultados se unen en una misma variable contador. Si son varios archivos leidos, las variables totales se suman para retornar una variable final con la suma de todas las características juntas.

# Especificación
La estructura del proyecto es la siguiente.
Se divide el trabajo en partes pequeñas para ahorrar tiempo al momento de ocntra las características.
Se unen las partes de trabajo realizado en la función contador
Se resuelven los problemas, como la condición de carrera creando una estructura con las variables caracter, variables y líneas, que se le asignan a un buffer que aumeta conforme los datos. Cada parte del trabajo realizado por cada hilo se guarda en su estructura, al final se suman dichos valores en una variable global. De esta forma se asegura que dos o más procesos estén en la misma sección crítica

# Problemas
La implementación del word count hace uso de varios hilos dependiendo del usuario, lo que hace más rápido el proceso de ocntar palabras. Sin embargo, el método para evitar conflicto entre estos procesos tiene sus esfuerzos y una gran cantidad de for y while, para archivos demasiado pesados, esta implementación no sería la más optima en cuanto al tiempo, en contra parte con otras soluciones. Reducir el uso de estos ciclos incrementaria de fomra significativa el tiempo de espera de los procesos.
