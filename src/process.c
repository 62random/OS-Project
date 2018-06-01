#include "process.h"

/**
	@brief			Função responsável por adicionar o resultado de um comando entre o mesmo e adiciona-lo a um pipe.
	@param  fp 		Porta para o ficheiro.
	@param  str		String com o resultado a escrever no ficheiro.
	@param	comando Struct com a informação sobre o comando executado e a sua descrição.
*/

char * escreveFicheiroAux(char * final,int * size,int * k, char * str , LCMD comando){
	int i = *k,x;
	int size1 = *size;
	char * aux;

	if (comando->desc != NULL){
		if ((size1-i) < strlen(comando->desc)){
			size1 += 2*strlen(comando->desc);
			aux = final;
			final = malloc(size1*sizeof(char));
			for(x = 0; x < i; x++)
				final[x] = aux[x];
			free(aux);
		}
		write(fp,comando->desc,strlen(comando->desc));
		write(fp,"\n",1);
	}

	/*
	if (comando->desc != NULL){
		write(fp,comando->desc,strlen(comando->desc));
		write(fp,"\n",1);
	}
	write(fp,comando->comando,strlen(comando->comando));
	write(fp,"\n",1);
	write(fp,">>>\n",4);
	write(fp,str,strlen(str)+1);
	write(fp,"<<<\n",4);*/
}

/**
	@brief					Função responsável por adicionar o resultado de vários comandos ligados a um pipe.
	@param  d_pai 			Porta para o ficheiro.
	@param  d_max_filho		Número de comandos interligados.
	@param	comando 		Struct com a informação sobre o comando executado e a sua descrição.
	@param 	buffer			Apontador de strings com os vários resultados dos comandos.
*/

char * juntaFildes(int d_pai,int d_max_filho,LCMD comando,char ** buffer){
	int i;
	int size = 100;
	char * str = malloc(size*sizeof(char));

	for(i = 0; i < d_max_filho; i++, comando = comando->prox){
		str = escreveFicheiroAux(str,&size,&k,buffer[i],comando);
	}
}

/**
	@brief				Função responsável por executar um conjunto de comandos interligados.
	@param  comando 	Lista de comandos a executar.
	@param	fd_origin 	Porta do pipe onde se vai guardar o resultado.
*/

int executa(LCMD comando,int fd_origin){
	int p[2];
	int n,c= 0, status;
	char ** args = NULL;
	LCMD aux_comando;
	char * str_aux;
	int size = 1024;
	char * buffer = malloc(size*sizeof(char));
	char x;
	int i = 0,k = 0;
	pid_t a;

	for(aux_comando = comando; aux_comando; aux_comando = aux_comando->prox)
		k++;

	char ** buffer_2 = malloc(k*sizeof(char *));

	for(aux_comando = comando; aux_comando; aux_comando = aux_comando->prox, c++){

		pipe(p);
        n = fork();
        if(n==0){
			dup2(p[1],1);
            close(p[1]);close(p[0]);
			args = split_string(aux_comando->comando);
			execvp(args[1],args + 1);
			perror("Fail no exec");
            _exit(-1);
        }
        else{
			close(p[1]); dup2(p[0],0);close(p[0]);
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
				a = WEXITSTATUS(status);
				if (a == 255)
					_exit(-1);
        	}
		}
	}
	juntaFildes(fd_origin,c,comando,buffer_2);

	_exit(0);
}

/**
	@brief			Função responsável por atribuir um tipo a uma string.
	@param source	String a avaliar.
	@return			Tipo da string.
*/

int type(char * source){
	if (source && strlen(source) > 3){
		if (source[1] == 'n')
			return 1;
		if (source[1] == ' ')
			return 2;
	}
	return 0;
}

/**
	@brief			Função responsável por calcular o comprimento de uma lista
	@param 		a	Lista a percorrer.
	@return			Tamanho do lista.
*/

int length(LCMD a){
	int r = 0;
	while (a) {
		r++;
		a = a->prox;
	}

	return r;
}

/**
	@brief				Função responsável por calcular em todos os comandos os indices em está o n-ésimo elemento.
	@param 		l		Array com as listas dos comandos.
	@param		pos 	Posição do array em que se começa a ler.
	@param		n		Número de elementos.
	@param		ele 	Local para retornar o indices das colunas
	@return				Indice no array em que está o elemento.
*/

int posicaoArray(LCMD * l, int pos, int n,	int * ele){

	int i,flag = 0,tam;
	int i_array = -1, i_col = -1;

	for(i=pos-1; i >= 0 && (!flag) ; i--){
		tam = length(l[i]);
		if ((n - tam) > 0){
			n -= tam;
		}
		else{
			flag = 1;
			i_array = i;
			i_col = tam - n;
		}
	}
	*ele = i_col;

	return i_array;
}

/**
	@brief				Função responsável por calcular o número que está no input.
	@param 	source		String com o comando.
	@return				Número a ser lido.
*/

int n_comando(char * source){
	char * buffer = NULL;
	int i=1;
	int r= -1;

	if (type(source) == 1){
		buffer = malloc(100*sizeof(char));
		while(source[i] != '|'){
			buffer[i-1] = source[i];
			i++;
		}
		buffer[i]='\0';
		r = atoi(buffer);
	}

	return r;
}
