#include "header.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdio.h>

typedef struct comando {
	char * comando;
	struct comando * prox;
} * COMANDO;

typedef struct linha {
	COMANDO linha;
	struct linha * prox;
} * LINHA;

int main(int argc, char const *argv[]) {

	int p[2];
	int i = 0;
	char ** args;
	int n;
	char buffer;

	if (argc != 2) {
		perror("Numero de argumentos inválido");
		exit(-1);
	}

	int fd = open(argv[1], O_RDWR | O_CREAT, 00644);
	COMANDO comandos = parse(fd);

	for(COMANDO aux = comandos; aux ; aux = aux->prox )
		i++;

	int fim = i;
	for(i = 0; comandos ; comandos = comandos->prox ){

		pipe(p);
	    n = fork();
	    if(n==0){
	     	if (i != fim - 1 )
	        	dup2(p[1],1);
	        close(p[1]);close(p[0]);
			args = processa(comandos->comando);
	        execvp(args[0],args);    // args direitos e its working I guess
	        _exit(-1);
	        }
	        else{
	            dup2(p[0],0);
				printf("Comando : \n" );
				while(read(0,&buffer,1) > 0)
					write(1,buffer,1);
	            close(p[0]);close(p[1]);
	        }

		    for(i=0;i<fim;i++)
		        wait(NULL);
	}


///falta muita merda




	return 1;
}
