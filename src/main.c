#include "parser.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdio.h>



#define TAM_STR 50
#define TAM_STR_MAIN 100




void executa(LCMD comando){

	int p[2];
	int i = 0, n;
	char ** args;
	LCMD aux;
	int c = 0;

	for(aux = comando; aux; aux = aux->prox )
		i++;

	for(aux = comando; aux; aux = aux->prox ){

        pipe(p);
        n = fork();
        if(n==0){
            if (c != i-1)
                dup2(p[1],1);
            close(p[1]);close(p[0]);

			args = split_string(aux->comando);
			execvp(args[1],args + 1);
				perror("Fail no exec");
            	_exit(-1);
        }
        else{
            dup2(p[0],0);
            close(p[0]);close(p[1]);
			wait(NULL);
			c++;
        }

	}



}

int main(int argc, char const *argv[]) {


	if (argc != 2) {
		perror("Numero de argumentos inválido");
		exit(-1);
	}

	int fd = open(argv[1], O_RDWR | O_CREAT, 00644);

	LCMD aux = parser(fd);
	int r = 0;
	LCMD * comandos;
	comandos = parser_split	(aux,  &r);

	for(int d = 0; d < r; d++){
		if (!fork()){
      		executa(comandos[d]);
			exit(0);
		}
	}

	for(int d = 0; d < r; d++){
		wait(NULL);
	}




	return 1;
}
