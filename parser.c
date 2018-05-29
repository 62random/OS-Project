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

LCMD criarCMD(char * src){
	LCMD novo = malloc(sizeof(struct linhacmd));
	novo->comando = src;
	novo->prox = NULL;

	return novo;
}

int split_string(char * str){
	if (strlen(str) >= 2 && str[0] == '$' && str[1] != '|')
		return 1;
	return 0;
}

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

LCMD * parser_split(LCMD a, int * r){
	LCMD aux = a,ant = NULL;
	int counter = 0;
	while (aux) {
		if(split_string(aux->comando))
			counter++;
		aux = aux->prox;
	}
	LCMD * final = malloc(counter*sizeof(struct linhacmd));

	counter = 0;

	while(a){
		if (split_string(a->comando)){
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
