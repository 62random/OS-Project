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
#define PATCH_TMP "/tmp/aux"





void executa(LCMD comando, int aux_2){
	int fd1;
	int p[2];
	int i = 0, n;
	char ** args;
	LCMD aux;
	char str_aux[1000];
	char buffer;

	int c = 0;
	for(aux = comando; aux; aux = aux->prox )
		i++;

	for(aux = comando; aux; aux = aux->prox ){

        pipe(p);
        n = fork();
        if(n==0){

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

			sprintf(str_aux,"%s/aux_%d_%d",PATCH_TMP,aux_2,c);
			fd1 = open(str_aux , O_WRONLY | O_TRUNC | O_CREAT , 00644);
			while ( (n = read(0,&buffer,1)) > 0)
    			write(fd1,&buffer,n);
			close(fd1);
			fd1 = open(str_aux , O_RDONLY, 00644);

			c++;
			dup2(fd1,0);
			wait(NULL);
        }

	}



}

int main(int argc, char const *argv[]) {


	if (argc != 2) {
		perror("Numero de argumentos inválido");
		exit(-1);
	}

    mkdir(PATCH_TMP, 0777	);



	int fd = open(argv[1], O_RDWR | O_CREAT, 00644);

	LCMD aux = parser(fd);
	int r = 0;
	LCMD * comandos;
	comandos = parser_split	(aux,  &r);

	for(int d = 0; d < r; d++){
		if (!fork()){
      		executa(comandos[d],d);
			exit(0);
		}
	}

	for(int d = 0; d < r; d++){
		wait(NULL);
	}
	// fazer output

	if (!fork()){
		execlp("rm","rm","-r","/tmp/aux",NULL);
		perror("Fallha ao dar delete");
		_exit(-1);
	}
	return 1;
}
