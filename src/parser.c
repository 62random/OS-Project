#include "parser.h"


typedef struct string_matrix{
	int size;
	int ocupados;
	char ** matrix;
} * MSTRING;

/**
	@brief			Função responsável inicializar um apontador para strings.
	@param  size 	Tamanho do apontador para strings.
	@return 		Estrutura com o apontador para strings.
*/

static MSTRING initMS(int size){
	MSTRING aux = malloc(sizeof(struct string_matrix));
	aux->size = size;
	aux->ocupados = 0;
	aux->matrix = malloc(size*sizeof(char *));

	return aux;
}

/**
	@brief			Função responsável por adicionar um elemento a uma apontador para strings.
	@param	str 	Adicionar uma apontador para strings.
	@param  ms 		Estrutura com a apontador para strings.
*/

static void addMatrix(char * str, MSTRING ms){
	if (!ms)
		return;

	char ** aux2;

	if (ms->size == ms->ocupados){
		aux2 = ms->matrix;
		ms->size *= 2;
		ms->matrix = malloc(ms->size*sizeof(char *));

		for(int i = 0; i < ms->ocupados; i++)
			ms->matrix[i] = aux2[i];
		free(aux2);
	}

	ms->matrix[ms->ocupados++] = str;
}

/**
	@brief			Função responsável por dar free a uma apontador para strings.
	@param  ms 		Estrutura com o apontador para strings.
*/

static void freeMString(MSTRING ms){
	int i;

	for(i = 0; i < ms->ocupados; i++)
		free(ms->matrix[i]);

	free(ms);
}

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
	@param  src1 	String inserir no comando.
	@param  src2 	String inserir no comando.
	@return 		Número de bytes lidos.
*/
LCMD criarCMD(char * src1 ,char * src2){
	char * aux;
	LCMD novo = malloc(sizeof(struct linhacmd));
	if (src1){
		aux = malloc((strlen(src1)+1)*sizeof(char));
		strcpy(aux,src1);
		novo->desc = aux;
	}
	else novo->desc = NULL;
	if (src2){
		aux = malloc((strlen(src2)+1)*sizeof(char));
		strcpy(aux,src2);
		novo->comando = aux;
	}
	else novo->comando = NULL;
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
	@brief			Função responsável verificar se a linha lida é um comando.
	@param  str 	String a verificar.
	@return 		Boolean.
*/

int test_dollar(char * str){
	if ((str) && str[0] == '$')
		return 1;
	return 0;
}

/**
	@brief			Função responsável ler de um ficheiro e converter o que foi lido num conjunto de comandos.
	@param  str 	Escritor do ficheiro.
	@return 		Conjunto de comandos.
*/

LCMD parser(int fildes){
	LCMD start = NULL, percorre = NULL, ant = NULL;
	MSTRING matrix = initMS(10);
	char str[200];
	char * str2= NULL;

	int n;
	while((n=readln(fildes,str,200)) > 0){
		str2 = malloc((n)*sizeof(char));
		strcpy(str2,str);
		addMatrix(str2,matrix);
	}

	char * str_ant = NULL;
	for(n = 0; n < matrix->ocupados; n++){
		if (test_dollar(matrix->matrix[n])){
			percorre = criarCMD(str_ant,matrix->matrix[n]);
			if (start == NULL)
				start = percorre;
			else{
				ant -> prox = percorre;
			}
			ant = percorre;
		}
		str_ant = matrix->matrix[n];
	}

	freeMString(matrix);

	percorre = start;
	
	while(percorre){
		if (test_dollar(percorre->desc)){
			str_ant = percorre->desc;
			percorre->desc = NULL;
			free(str_ant);
		}
		percorre = percorre -> prox;
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

/**
	@brief			Função responsável por contar o número de palavras de uma string.
	@param  str 	String a percorrer.
	@return 		Número de palavras.
*/

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

/**
	@brief			Função responsável por partir uma string num conjunto de strings das suas palavras.
	@param  str 	String a percorrer.
	@return 		Array de string com as palavras da string.
*/

char ** split_string(char * str){
	char s[2] = " ";
	char * token;

	int size = wordcount(str);

	char ** matrix = malloc((size)*sizeof(char *));

	token = strtok(str, s);

	int i = 0;

	while(token != NULL){
		matrix[i++] = token;
		token = strtok(NULL, s);
	}
	matrix[i] = NULL;

	return matrix;
}

/**
	@brief			Função responsável por dar free a um apontador para strings.
	@param  str 	Apontador para strings
*/

void freeApChar(char ** str){
	if (str){
		int i = 0;
		char * aux;
		while (str[i]) {
			aux = str[i++];
			free(aux);
		}
		free(str);
	}
}
