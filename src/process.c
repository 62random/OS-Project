#include "process.h"

void escreveFicheiroAux(int fp, char * str , LCMD comando){

	if (comando->desc != NULL){
		write(fp,comando->desc,strlen(comando->desc));
		write(fp,"\n",1);
	}
	write(fp,comando->comando,strlen(comando->comando));
	write(fp,"\n",1);
	write(fp,">>>\n",4);
	write(fp,str,strlen(str)+1);
	write(fp,"<<<\n",4);
}

void juntaFildes(int d_pai,int d_max_filho,LCMD comando,char ** buffer){
	int i;

	for(i = 0; i < d_max_filho; i++, comando = comando->prox){
		escreveFicheiroAux(d_pai,buffer[i],comando);
	}
}

void printmatrix(char ** matrix){
	int i=0;
	while (matrix[i]) {
		printf("%s\n",matrix[i]);
		i++;
	}
}

void executa(LCMD comando, int aux_2,int fd_origin){
	int p[2];
	int n,c= 0, status;
	char ** args;
	LCMD aux;
	char * str_aux;
	int size = 1024;
	char * buffer = malloc(size*sizeof(char));
	char x;
	int i = 0,k = 0;

	for(aux = comando; aux; aux = aux->prox)
		k++;
	char ** buffer_2 = malloc(k*sizeof(char *));

	for(aux = comando; aux; aux = aux->prox, c++){

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
			close(p[1]);
			dup2(p[0],0);
			close(p[0]);
			while (read(0,&x,1) > 0){
				buffer[i] = x;
				i++;
				if (i == size){
					str_aux = buffer;
					buffer = malloc(size*2*sizeof(char));
					for(k = 0; k < size; k++){
						buffer[k] = str_aux[k];
					}
					free(str_aux);
					size *= 2;
				}
			}
			pipe(p);
			buffer[i] = '\0';
			write(p[1],buffer,i);

			str_aux = malloc((i+1)*sizeof(char));
			strcpy(str_aux,buffer);
			buffer_2[c] = str_aux;

			i = 0;
			close(p[1]); dup2(p[0],0); close(p[0]);

			wait(&status);
			if (WIFEXITED(status)){
				if (WEXITSTATUS(status) == -1)
					_exit(-1);
        	}
		}
	}
	juntaFildes(fd_origin,c,comando,buffer_2);
}
