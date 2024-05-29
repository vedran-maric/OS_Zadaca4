#include <semaphore.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

/*Ulazak na vrtuljak*/
void posjetitelj(int i){

	sem_t* mjesta = sem_open("mjesta", 0);
	while(1){
		sem_wait(mjesta);
		printf("proces %d je usao u vrtuljak\n", i);
		sleep(3);
	}	
}

/*Rad vrtuljka*/
void vrtuljak(){
	int broj_vrtnji = 0;
	sem_t* mjesta = sem_open("mjesta", 0); //otvaranje semafora
	int * slobodna_mjesta = malloc(sizeof(int));

	/*Vrti petlju dok su slobodna mjesta*/
	while(1){
		do{
			sem_getvalue(mjesta, slobodna_mjesta);			
			printf("broj slobodnih mjesta: %d\n", *slobodna_mjesta);
			sleep(1);
		}while(*slobodna_mjesta != 0);
	
		/*Vrtnja vrtuljka*/
		sleep(1);
		printf("vrtuljak se vrti %d. put\n", ++broj_vrtnji);
		sleep(3);
		printf("Vrtuljak se zaustavio\n");

		/*Oslobodi 5 mjesta*/
		for(int i = 1; i <= 5; i++){
			sem_post(mjesta);
			printf("Sjedalo br %d na vrtuljku se ispraznilo!\n", i);
		}
	}
}


void main(){
    /*Kreiranje semafora*/
	sem_t* mjesta = sem_open("mjesta", O_CREAT, 0777, 0);
	
	for(int i = 0; i < 5; i++){
		sem_post(mjesta);
	}
	
	/*Pokretanje vrtuljka i paralelnih procesa za svakog posjetitelja*/
	if(fork() == 0){
		vrtuljak();
	}
	
	sleep(1);
	
	for(int i = 0; i < 15;i++){
		if(fork() == 0){
			posjetitelj(i);
		}
	}
	
	/*Gasenje procesa i oslobadjanje memorije*/
	for(int i = 0; i < 16; i++){
		wait(NULL);
	}
	
	sem_destroy(mjesta);

}
  