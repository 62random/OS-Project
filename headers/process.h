#ifndef __PROCESS__
#define __PROCESS__

#include "parser.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdio.h>

void escreveFicheiroAux		(int fp, char * str , LCMD comando);
void juntaFildes			(int d_pai,int d_max_filho,LCMD comando,char ** buffer);
void printmatrix			(char ** matrix);
void executa				(LCMD comando, int aux_2,int fd_origin);

#endif
