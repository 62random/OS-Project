#include "parser.h"
#include "process.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdio.h>



#define TAM_STR 50
#define TAM_STR_MAIN 100


int main(int argc, char const *argv[]) {

	if (argc != 2) {
		perror("Numero de argumentos inválido");
		exit(-1);
	}

	int fd = open(argv[1], O_RDWR | O_CREAT, 00644);

	if (fd == -1){
		perror("Não conseguiu abrir a porta do ficheiro.");
		_exit(-1);
	}

	LCMD aux = parser(fd);
	int r = 0,d,fd1,n,status;
	LCMD * comandos = parser_split	(aux,  &r);
	close(fd);
	int p[2],v[r];

	for(d = 0; d < r; d++){
		pipe(p);
		if (!fork()){
			close(p[0]);
      		executa(comandos[d],d,p[1]);
			close(p[1]);
			exit(0);
		}
		else{
			v[d] = p[0];
			close(p[1]);
		}
	}

	for(d = 0; d < r; d++){
		wait(&status);
	}

	fd = open(argv[1], O_WRONLY | O_TRUNC, 00644);
	if (fd == -1){
		perror("Não conseguiu abrir a porta do ficheiro.");
		_exit(-1);
	}

	char c;

	for(d = 0; d < r; d++){
		fd1 = v[d];

		while ((n = read(fd1,&c,1)) > 0){
			write(fd,&c,n);
		}
		close(fd1);
	}


	return 1;
}
