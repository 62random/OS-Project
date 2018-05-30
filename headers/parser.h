#ifndef __PARSER__
#define __PARSER__

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct linhacmd{
	char * comando;
	struct linhacmd * prox;
} * LCMD;



int 	readln			(int fildes, char *buf, int nbyte);
LCMD 	criarCMD		(char * src);
int 	split_string	(char * str);
LCMD 	parser			(int fildes);
LCMD * 	parser_split	(LCMD a, int * r);

#endif
