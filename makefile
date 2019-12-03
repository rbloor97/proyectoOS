myshell: myshell.o
	gcc -o myshell myshell.o 

myshell.o: myshell.c 
	gcc -c  myshell.c 
clean:
	-rm -f myshell.o
	-rm -f myshell 
