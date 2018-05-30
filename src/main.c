<<<<<<< HEAD
=======
#include "header.h"
>>>>>>> b0a1307d454e212bc2368d8d399b1b40c8b56511
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




<<<<<<< HEAD
int wordcount(char * str){
	int status = 0, counter = 0,i;

	for(i=0; str[i] != '\0'; i++)
		if (status == 0 && str[i] != ' '){
			status = 1;
			counter++;
		}
		else if (status == 1 && str[i] == ' ')
			status = 0;

	return counter;
}


int mysystem(const char * command){

   int p = fork();

   if (p == 0) {
	   int n = conta_palavras(command); 				//contar palavras

	   int v[n],i; 										//array com o tam das palavra
	   for(i=0; i < n; v[i++]=0); 						//inicializar o array

	   char ** args = malloc((n+1)*sizeof(char*)); 		//alocar espaço para os argumentos
	   for(i=0; i <= n; i++)
		   args[i] = malloc(TAM_STR*sizeof(char));

	   args[n] = NULL;
=======
void executa(LCMD comando){
>>>>>>> b0a1307d454e212bc2368d8d399b1b40c8b56511

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

	int fd = open("note", O_RDWR | O_CREAT, 00644);

	LCMD aux = parser(fd);
	int r = 0;
	LCMD * comandos;
	comandos = parser_split	(aux,  &r);

	for(int d = 0; d < r; d++){
		if (!fork())
      		executa(comandos[d]);
	}

	for(int d = 0; d < r; d++){
		wait(NULL);
	}


<<<<<<< HEAD
	LCMD linhas = parse(fd);
=======
>>>>>>> b0a1307d454e212bc2368d8d399b1b40c8b56511



	return 1;
}
