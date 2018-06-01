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
int * paused_process;
int n_paused_process;
int n_paused_size;

void removeFiles(){
	if(!fork()){
		execlp("rm","rm","-r",LOCAL,NULL);
	}
}

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

void kill_all(int i){

	pid_t self = getpid();
    if (daddy == self){
		for(int j = 0; j < n_paused_process; j++){
			kill(SIGCONT,paused_process[j]);
		}
	}

}


int main(int argc, char const *argv[]) {
	int fork_pid_aux;
	n_paused_size = 10;
	n_paused_process = 0;
	paused_process = malloc(sizeof(int) * n_paused_size);
	daddy = getpid();
	signal(SIGINT,kill_all);
	signal(SIGALRM,lidar_alarm);


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
	pid_t a;
	int linha,coluna,n_com, v[r];

	if(!calculaDependencias(comandos,v,r)){
		perror("Um dos comandos do tipo $n| não pode ser executado.");
		_exit(-1);
	}
	mkdir(LOCAL,0777);
	for(d = 0; v[d]; d++)
		printf("%d\n",v[d] );

	//fd = open(argv[1], O_RDONLY , 00644);

	for(d = 0; d < r; d++){
		/*
		char *blasd = parseFileToString(2,fd);
		if (type(comandos[d]->comando) == 1){
			n_com = n_comando(comandos[d]->comando);
			linha = posicaoArray(comandos,d,n_com,&coluna);
			printf("n:%d -> linha:%d -> coluna:%d\n",n_com,linha,coluna);
		}*/


		if(type(comandos[d]->comando) == 1){
			fork_pid_aux = fork();
			if(fork_pid_aux){
				if(++n_paused_process >= n_paused_size){
					free(paused_process);
					n_paused_size *= 2;
					paused_process = malloc(sizeof(int) * n_paused_size);
				}
				paused_process[n_paused_process++] = fork_pid_aux;
			}
			//funcao ambrosio com output
			executa_n(comandos[d],d,input);

		}
		else{
			if(!fork()){
				executa(comandos[d],d);
				_exit(0);
			}
		}
	}

	for(d = 0; d < r; d++){
		wait(&status);
		if (WIFEXITED(status)){
			a = WEXITSTATUS(status);
			if (a == 255){
				removeFiles();
				_exit(-1);
			}
		}
	}

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
			removeFiles();
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
