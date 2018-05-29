#include <stdio.h>
#include <fcntl.h>

typedef struct comando 	* COMANDO;
typedef struct linha 	* LINHA;

char ** processa(char *);
COMANDO parse(int);
