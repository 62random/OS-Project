#include "header.h"

typedef struct comando {
	char * comando;
	struct comando * prox;
} * COMANDO;

typedef struct linha {
	COMANDO linha;
	struct linha * prox;
} * LINHA;

int main(int argc, char const *argv[]) {

	if (argc != 2) {
		perror("Numero de argumentos inválido");
		exit(-1);
	}

	int fd = open(argv[1], O_RDWR | O_CREAT, 00644);


	LINHA linhas = parse(fd);



	return 1;
}
