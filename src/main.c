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
#define PATCH_TMP "/tmp/filesSO_work"


void escreveFicheiroAux(int fp, int fp_aux, LCMD comando){
	int n;

	char c;

	if (comando->desc != NULL){
		write(fp,comando->desc,strlen(comando->desc));
		write(fp,"\n",1);
	}
	write(fp,comando->comando,strlen(comando->comando));
	write(fp,"\n",1);
	write(fp,">>>\n",4);

	while ((n=read(fp_aux,&c,1)) > 0) {
		write(fp,&c,n);
	}
	write(fp,"<<<\n",4);
	close(fp_aux);
}


void juntaFildes(int d_pai,int d_max_filho,LCMD comando){

	char str[100], str_aux[100];

	sprintf(str,"%s/aux_final_%d",PATCH_TMP,d_pai);// criar o final;

	int fp = open(str,O_APPEND | O_CREAT | O_WRONLY,0644),fp_aux, i;

	if (fp == -1){
		perror("Não conseguiu abrir a porta para o ficheiro final");
		_exit(-1);
	}

	for(i = 0; i < d_max_filho; i++, comando = comando->prox){
		sprintf(str_aux,"%s/aux_%d_%d",PATCH_TMP,d_pai,i);
		fp_aux = open(str_aux,O_RDONLY,0644);

		if (fp_aux == -1){
			perror("Erro ao abrir o Escritor de ficheiro\n");
			_exit(-1);
		}
		escreveFicheiroAux(fp,fp_aux,comando);
	}

}


void executa(LCMD comando, int aux_2){
	int p[2];
	int n, c= 0, fd1;
	char ** args;
	LCMD aux;
	char str_aux[1000];
	char buffer;

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

			while ( (n = read(0,&buffer,1)) > 0){
    			write(fd1,&buffer,n);
			}

			close(fd1);
			fd1 = open(str_aux , O_RDONLY, 00644);

			c++;
			dup2(fd1,0);
			wait(NULL);// preciso testar se dá merda para parar.
        }
	}
	juntaFildes(aux_2,c,comando);
}

int main(int argc, char const *argv[]) {


	if (argc != 2) {
		perror("Numero de argumentos inválido");
		exit(-1);
	}

    mkdir(PATCH_TMP, 0777);

	int fd = open(argv[1], O_RDWR | O_CREAT, 00644);

	if (fd == -1){
		perror("Não conseguiu abrir a porta do ficheiro.");
		_exit(-1);
	}

	LCMD aux = parser(fd);
	int r = 0,d,fd1,n;
	LCMD * comandos = parser_split	(aux,  &r);
	close(fd);
	for(int i = 0; i < r; i++){
		aux = comandos[i];
		while(aux){
			printf("%s\n",aux->comando);
			aux = aux ->prox;
		}
		printf("next\n");
	}

	for(d = 0; d < r; d++){
		if (!fork()){
      		executa(comandos[d],d);
			exit(0);
		}
	}

	for(d = 0; d < r; d++){
		wait(NULL);
	}

	fd = open(argv[1], O_WRONLY | O_TRUNC, 00644);
	if (fd == -1){
		perror("Não conseguiu abrir a porta do ficheiro.");
		_exit(-1);
	}

	char str [100],c;
	printf("1\n");

	for(d = 0; d < r; d++){
		sprintf(str,"%s/aux_final_%d",PATCH_TMP,d);
		fd1 = open(str,O_RDONLY,0644);

		if (fd1 == -1){
			perror("Não conseguiu abrir a porta do ficheiro.");
			_exit(-1);
		}

		while ((n = read(fd1,&c,1)) > 0){
			write(fd,&c,n);
		}
	}




	if (!fork()){
		execlp("rm","rm","-r",PATCH_TMP,NULL);
		perror("Fallha ao dar delete");
		_exit(-1);
	}
	return 1;
}
