#include "parser.h"
#include "process.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdio.h>
#include <signal.h>



#define TAM_STR 50
#define TAM_STR_MAIN 100
#define RUNNING 1
#define PAUSED 0
#define FINISHED 2
#define NOSTARTED -1

pid_t daddy;
char * buffer_safe_mode;
int size;
int k;
char const * filesource;
int * process_status;
int * process_link;
int * dependencias;
int r;

void removeFilesW();

/**
	@brief			Função responsável por criar um backup do ficheiro inicial.
	@param  source 	Path para o file
*/

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

void removeFiles(){
	if(!fork()){
		execlp("rm","rm","-r",LOCAL,NULL);
	}
}

/**
	@brief			Função responsável escrever o backup no ficheiro.
*/

void backup_write(){
	int fd = open(filesource, O_WRONLY | O_TRUNC , 00644);

	if (fd == -1){
		perror("Não conseguiu abrir a porta do ficheiro.");
		_exit(-1);
	}

	write(fd,buffer_safe_mode,k);
	close(fd);
}

/**
	@brief			Função responsável matar todos os processos menos o pai.
	@param  i 		Tipo de sinal.
*/

void kill_all(int i){

	pid_t self = getpid();
    if (daddy != self) _exit(-1);
	else{
		printf("A sair de todos os processos\n" );
		if(!fork()){
			execlp("rm","rm","-r",LOCAL,NULL);
		}
		wait(NULL);
		backup_write();
		_exit(0);
	}

}

void wakeup(int i){
	int status,pid,a,d;

	pid = wait(&status);

	if (pid != -1){
		if (WIFEXITED(status)){
			a = WEXITSTATUS(status);
			if (a == 255){
				removeFiles();
				_exit(-1);
			}
		}
		a=0;
		while (process_link[a] != pid && a < r){
			a++;
		}
		d = a;
		process_status[d] = FINISHED;
		for(a = 0; a < r; a++){
			if(dependencias[a] == d){
				kill(process_link[a],SIGCONT);
				process_status[a] = RUNNING;
			}
		}
	}
}

void removeFilesW(){
	backup_write();
	if(!fork()){
		execlp("rm","rm","-r",LOCAL,NULL);
	}
}

int verificaArray(){
	int i = 0;
	while(i < r && process_status[i] == 2){
		i++;
	}
	return (i != r);
}


int main(int argc, char const *argv[]) {

	signal(SIGINT,kill_all);
	signal(SIGCHLD,wakeup);

	int fork_pid_aux;

	daddy = getpid();

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
	int d,fd1,n,status;
	LCMD * comandos = parser_split(aux,&r);
	close(fd);
	pid_t a;
	int linha,coluna;

	dependencias = malloc(sizeof(int) * r);

	if(!calculaDependencias(comandos,dependencias,r)){
		perror("Um dos comandos do tipo $n| não pode ser executado.");
		_exit(-1);
	}
	mkdir(LOCAL,0777);

	process_status = malloc(sizeof(int) * r);
	process_link = malloc(sizeof(int)*r);

	char * input;

	for(d = 0; d < r; d++){
		process_status[d] = NOSTARTED;
		if(type(comandos[d]->comando) == 1){ // n-esimo
			fork_pid_aux = fork();
			if(fork_pid_aux){
				process_link[d] = fork_pid_aux;
				process_status[d] = PAUSED;
				kill(fork_pid_aux,SIGSTOP);
			}
			else{
				linha = posicaoArray(comandos, d, n_comando(comandos[d]->comando), &coluna);
				input = outputFromFile(linha, coluna);
				executa_n(comandos[d],d,input);
				_exit(0);
			}
		}
		else{ // normal
			fork_pid_aux = fork();
			if(!fork_pid_aux){
				executa(comandos[d],d);
				_exit(0);
			}
			else{
				process_link[d] = fork_pid_aux;
				process_status[d] = RUNNING;
			}
		}
	}

	/*
	for(d = 0; d < r; d++){
		wait(&status);
		if (WIFEXITED(status)){
			a = WEXITSTATUS(status);
			if (a == 255){
				removeFiles();
				_exit(-1);
			}
		}
	}*/
	while(verificaArray());

	fd = open(argv[1], O_WRONLY | O_TRUNC, 00644);
	if (fd == -1){
		perror("Não conseguiu abrir a porta do ficheiro.");
		removeFiles();
		_exit(-1);
	}
	char c;
	char str [100];

	for(d = 0; d < r; d++){
		sprintf(str,"%s/aux_%d",LOCAL,d);

		fd1 = open(str,O_RDONLY,0644);

		if (fd1 == -1){
			perror("Não conseguiu abrir a porta do ficheiro.");
			removeFilesW();
			_exit(-1);
		}

		while ((n = read(fd1,&c,1)) > 0){
			write(fd,&c,n);
		}
		close(fd1);
	}

	removeFiles();

	return 1;
}
