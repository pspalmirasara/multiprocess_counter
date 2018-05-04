#include <stdio.h>
#include <stdlib.h> /* exit() */
#include <sys/types.h> /* define pid_t */
#include <sys/wait.h>
#include <unistd.h> /* fork() */
#include <sys/mman.h> /* biblioteca para usar uma variavel global*/


#define N_PROCESSOS 3

static int *glob_var; //variavel global onde ficarao quantos numeros primos existem

void verifica_primo(long int n){
	int aux;
	int eh_primo = 1; //0 caso nao seja primo
			  //1 caso seja primo

	if (n==1 || n==0){
		eh_primo = 0;
		return;
	}

	for (int i = 2; i<n; i++){
		aux = n % i;
		if (aux == 0){
			eh_primo = 0;
			break;
		}
	}
	if (eh_primo == 1){
		(*glob_var)++;
	}

	return;
}


int main() {
	pid_t filho[N_PROCESSOS];
	long int entrada[20]; //as entradas ficarao nesse vetor
	char entrada_char;    //variavel auxiliar para receber a entrada
	int i_entrada; 	      //indice do vetor da entrada
	int i_filho;          //indice do vetor dos filhos


	glob_var = mmap(NULL, sizeof *glob_var, PROT_READ | PROT_WRITE, 
                    MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	*glob_var = 0;

	unsigned int f;

	/*Recebendo a entrada*/
	int N_entradas = 0; //tamanho total do vetor de entradas
	while (1){
		scanf("%li", &entrada[N_entradas]);
		N_entradas++;
		scanf("%c", &entrada_char);
		if (entrada_char != ' '){
			break;
		}
	}

	
	/*Multiprocesso*/
	i_filho = 0;
	for (i_entrada = 0; i_entrada<N_entradas; i_entrada++){		
		filho[i_filho] = fork();
		if (filho[i_filho] == 0) { //execucao dos filhos
			verifica_primo(entrada[i_entrada]);
			exit(0);
		}

		if( (i_filho==0) ||  (i_filho==1) || (i_filho==2) ){
			i_filho++;
		}else{
			/*Esperando todos os filhos terminarem os processos caso todos estejam ocupados*/
			for (int k=0; k<N_PROCESSOS; k++) {
				waitpid(filho[k], NULL, 0);
			}
			i_filho = 0; //volta a ocupar o primeiro filho
		}
	}

	/*Esperando todos os filhos terminarem*/
	for (int i=0; i<N_PROCESSOS; i++) {
		waitpid(filho[i], NULL, 0);
	}

	printf ("%d\n", (*glob_var));


	return 0;
}
