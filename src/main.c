#include "header.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdio.h>



#define TAM_STR 50
#define TAM_STR_MAIN 100



typedef struct comando {
	char * comando;
	struct comando * prox;
} * COMANDO;

typedef struct linha {
	COMANDO linha;
	struct linha * prox;
} * LINHA;



int conta_palavras(const char * line){
   int i = 0;
   int palavra = 0,status = 0;
   for(;line[i] != '\0'; i++){
	   if (status == 0 && line[i] != ' ') {
		   palavra++; status = 1;
	   }
	   else if (status == 1 && line[i] == ' '){
		   status=0;
	   }
   }

   return palavra;
}

void escreve_conta_palavras(const char * line,int *v,int n,char ** args){
   int i = 0;
   int palavra = 0,status = 0;
   for(;line[i] != '\0'; i++){
	   if (status == 0 && line[i] != ' ') {
		   palavra++;
		   status = 1;
		   args[palavra-1][v[palavra-1]] = line[i];
		   v[palavra-1]++;
	   }
	   else if (status == 1 && line[i] == ' '){
		   status=0;
	   }
	   else if (status == 1){
		   args[palavra-1][v[palavra-1]] = line[i];
		   v[palavra-1]++;
	   }
   }
   for(i=0; i < palavra; i++)
	   args[i][v[i]] ='\0';
}


int mysystem(const char * command){

   int p;

   p=fork();

   if (p == 0) {
	   int n = conta_palavras(command); // contar palavras

	   int v[n],i; // array com o tam das palavra
	   for(i=0; i < n; v[i++]=0); // inicializar o array

	   char ** args = malloc((n+1)*sizeof(char*)); // alocar espaço para os argumentos
	   for(i=0; i <= n; i++){
		   args[i] = malloc(TAM_STR*sizeof(char));
	   }
	   args[n] = NULL;


	   escreve_conta_palavras(command,v,n,args); // preencher os argumentos

	   execvp(args[0],args); //executar
	   perror("erros->bla");
	   _exit(0);

   }

   return 0;
}

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
	    if(!n){
	     	if (i != fim - 1 )
	        	dup2(p[1],1);
	        close(p[1]);close(p[0]);
			mysystem(comandos->comando);
			//args = processa(comandos->comando);
	        //execvp(args[0],args);    // se args direitos e its working I guess
				perror("falhou exec, culpa do random fdp");
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






	return 1;
}
