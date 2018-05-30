#include "parser.h"

typedef struct linhacmd{
	char * comando;
	struct linhacmd * prox;
} * LCMD;

/**
	@brief			Função responsável por ler uma linha de um ficheiro.
	@param  fildes 	Escritor do ficheiro.
	@param  buf	   	Apontador para onde vai ser lido.
	@param  nbyte	Número de elementos máximos do array.
*/

int readln(int fildes, char *buf, int nbyte){
    int num = 0,n,i;
    char c = *buf;

    while( c != '\n' && num < nbyte){
        n = read(fildes,buf+num,1);
		if (n == 0)
			break;
		num += n;
        c = *(buf + num-1);
    }
	for(i = 0 ; i < num; i++)
		if ((*(buf + i)) == '\n'){
			*(buf + i) = '\0';
			break;
		}
    return num;
}

/**
	@brief			Função responsável alocar memória para uma linha de comandos.
	@param  src 	String inserir no comando.
	@return 		Número de bytes lidos.
*/

LCMD criarCMD(char * src){
	LCMD novo = malloc(sizeof(struct linhacmd));
	novo->comando = src;
	novo->prox = NULL;

	return novo;
}

/**
	@brief			Função responsável verificar o tipo de comando inseriro.
	@param  str 	String a verificar.
	@return 		Boolean.
*/

int type_string(char * str){
	if (strlen(str) >= 2 && str[0] == '$' && str[1] != '|')
		return 1;
	return 0;
}

/**
	@brief			Função responsável ler de um ficheiro e converter o que foi lido num conjunto de comandos.
	@param  str 	Escritor do ficheiro.
	@return 		Conjunto de comandos.
*/

LCMD parser(int fildes){
	LCMD start = NULL, percorre, ant = NULL;

	char str[200];
	char * str2;

	int n;
	while((n=readln(fildes,str,200)) > 0){
		str2 = malloc((n+1)*sizeof(char));
		strcpy(str2,str);

		percorre = criarCMD(str2);

		if (ant == NULL){
			start = percorre;
			ant = percorre;
		}
		else{
			ant->prox = percorre;
			ant = percorre;
		}
	}
	return start;
}

/**
	@brief			Função responsável ler de um ficheiro e converter o que foi lido em conjuntos de comandos.
	@param  str 	Conjunto de comandos.
	@param  r 		Número de conjuntos criados.
	@return 		Array com o conjunto dos comandos do ficheiro.
*/


LCMD * parser_split(LCMD a, int * r){
	LCMD aux = a,ant = NULL;
	int counter = 0;
	while (aux) {
		if(type_string(aux->comando))
			counter++;
		aux = aux->prox;
	}
	LCMD * final = malloc(counter*sizeof(struct linhacmd));

	counter = 0;

	while(a){
		if (type_string(a->comando)){
			if (ant){
				ant->prox = NULL;
			}
			final[counter++] = a;
		}
		ant = a;
		a = a->prox;
	}
	*r = counter;

	return final;
}

int wordcount(char * str){
	int status = 0, counter = 0,i;

	for(i=0; str[i] != '\0'; i++){
		if (status == 0 && str[i] != ' '){
			status = 1;
			counter++;
		}
		else if (status == 1 && str[i] == ' '){
			status = 0;
		}
	}

	return counter;
}


char ** split_string(char * str){
	char s[2] = " ";
	char * token;

	int size = wordcount(str);

	char ** matrix = malloc((size)*sizeof(char *));

	token = strtok(str, s);

	int i = 0;

	while(token != NULL){
		if (i != 0)
			matrix[i-1] = token;
		token = strtok(NULL, s);
		i++;
	}
	matrix[i] = NULL;

	return matrix;
}
