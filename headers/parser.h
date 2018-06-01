#ifndef __PARSER__
#define __PARSER__

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct string_matrix{
	int size;
	int ocupados;
	char ** matrix;
} * MSTRING;

typedef struct linhacmd{
	char * desc;
	char * comando;
	struct linhacmd * prox;
} * LCMD;


MSTRING initMS			(int size);
void 	addMatrix		(char * str, MSTRING ms);
void 	freeMString		(MSTRING ms);
int 	readln			(int fildes, char *buf, int nbyte);
LCMD 	criarCMD		(char * src1,char * src2);
int 	type_string		(char * str);
LCMD 	parser			(int fildes);
LCMD * 	parser_split	(LCMD a, int * r);
char ** split_string	(char * str);
int 	wordcount		(char * str);
void 	freeApChar		(char ** str);


#endif
