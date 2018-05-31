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

pid_t daddy;
char * buffer_safe_mode;
int size;
int k;
char const * filesource;

void backup_read(char const * source){

	int fd = open(source, O_RDONLY , 00644);

	if (fd == -1){
		perror("Não conseguiu abrir a porta do ficheiro.");
		_exit(-1);
	}

	buffer_safe_mode = malloc(100*sizeof(char));
	size = 100;

	char c, * aux;
	int i = 0,n;

	while((n = read(fd,&c,1)) > 0){
		buffer_safe_mode[i++] = c;
		if (i == size){
			aux = buffer_safe_mode;
			buffer_safe_mode = malloc(2*size*sizeof(char));
			for(n = 0; n < i ; n++)
				buffer_safe_mode[n] = aux[n];
			free(aux);
			size *= 2;
		}
	}
	k = i;
	close(fd);
}

void backup_write(){
	int fd = open(filesource, O_WRONLY | O_TRUNC , 00644);

	if (fd == -1){
		perror("Não conseguiu abrir a porta do ficheiro.");
		_exit(-1);
	}

	write(fd,buffer_safe_mode,k);
	close(fd);
}

void kill_all(int i){

	pid_t self = getpid();
    if (daddy != self) _exit(-1);
	else{
		printf("A sair de todos os processos\n" );
		backup_write();
		_exit(0);
	}

}

int main(int argc, char const *argv[]) {
	daddy = getpid();
	signal(SIGINT,kill_all);

	if (argc != 2) {
		perror("Numero de argumentos inválido");
		exit(-1);
	}
	filesource = argv[1];
	backup_read(argv[1]);

	int fd = open(argv[1], O_RDONLY , 00644);

	if (fd == -1){
		perror("Não conseguiu abrir a porta do ficheiro.");
		_exit(-1);
	}

	LCMD aux = parser(fd);
	int r = 0,d,fd1,n,status;
	LCMD * comandos = parser_split(aux,&r);
	close(fd);
	int p[2],v[r];
	pid_t a;

	for(d = 0; d < r; d++){
		pipe(p);
		if (!fork()){
			close(p[0]);
      		executa(comandos[d],p[1]);
			close(p[1]);
			_exit(0);
		}
		else{
			v[d] = p[0];
			close(p[1]);
		}
	}

	for(d = 0; d < r; d++){
		wait(&status);
		if (WIFEXITED(status)){
			a = WEXITSTATUS(status);
			if (a == 255){
				perror("Um do comandos não conseguiu ser executado.");
				_exit(-1);
			}
		}
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
