#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <mqueue.h>
#include <string.h>
#include "paciente.h"
#include "cola.h"
#include "recepcion.h"

int pacientes_dados_de_alta = 0;

pid_t pid_hospital, pid_recepcion;

int tiempo_aleatorio(int min, int max) {//Esto da un tiempo aleatorio entre los dos numeros dados
    return rand() % (max - min + 1) + min;
}

void* exploracion(void* args) {
    printf("[Exploración] Comienzo mi ejecución...\n");
    while (1) {
        char paciente[128];
	printf("[Exploración] Esperando a un paciente...\n");//Ahora hay que registrar a un paciente
		printf("[Exploración] Recibido paciente: %s. Realizando exploración...\n", paciente);
		sleep(tiempo_aleatorio(1, 3));
		printf("[Exploración] Exploración completa. Notificando diagnóstico...\n");

		printf("No hay pacientes... Fiesta!! \n");



    }
}

void* diagnostico(void* args) {
    printf("[Diagnóstico] Comienzo mi ejecución...\n");
    while (1) {

        printf("[Diagnóstico] Realizando pruebas diagnósticas...\n");
        sleep(tiempo_aleatorio(5, 10));
        printf("[Diagnóstico] Diagnóstico completado. Notificando farmacia...\n");

    }
}


void* farmacia(void* args) {
    printf("[Farmacia] Comienzo mi ejecución...\n");
    while (1) {

        printf("[Farmacia] Preparando medicación...\n");
        sleep(tiempo_aleatorio(1, 3));
        printf("[Farmacia] Medicación lista. Enviando señal de alta...\n");

    }
}



void main(int argv, char* argc[]) {

	//pthread_t hilo_Diagnostico;//Creamos el hilo del diagnostico.

	//pthread_create(&hilo_Diagnostico, NULL, diagnostico, NULL);//Esto ejecuta el hilo del diagnostico y entra en bucle infinito.
	pid_recepcion = fork();


	if (pid_recepcion != 0) {
		pid_hospital = fork();
		if (pid_hospital != 0) {
			// Proceso padre



		} else {
			// Proceso hospital
			printf("[Hospital] Comienzo mi ejecución...\n");



		}
	} else {
		// Proceso recepción, no parece que necesite hilo
    		while (1) {//Aqui va la recepcion!!
        		char paciente[128];
    			sleep(tiempo_aleatorio(1, 10));
                printf("Detectado paciente, verificando solicitud de paciente...\n");


    		}
	}

	exit(0);
}
