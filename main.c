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

	pthread_t hilo_Diagnostico;//Creamos el hilo del diagnostico.

	pthread_create(&hilo_Diagnostico, NULL, diagnostico, NULL);//Esto ejecuta el hilo del diagnostico y entra en bucle infinito.
	pid_recepcion = fork();
	struct Paciente *lista_Espera_Pacientes = malloc(30 * sizeof(Paciente));//Puede haber hasta 30 pacientes en espera
	struct Paciente *lista_Pacientes = malloc(5 * sizeof(Paciente));//Puede haber hasta cinco pacientes en la lista de pacientes

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
		int n = 0;
    		while (1) {//Aqui va la recepcion!!
        		char paciente[128];
    			sleep(tiempo_aleatorio(1, 10));
				if (lista_Espera_Pacientes != NULL) {
                    printf("Detectado paciente, verificando solicitud de paciente...\n");
            		//struct Paciente *paciente = &lista_Espera_Pacientes[n];
                    //&lista_Pacientes[n] = *paciente;
                    n++;
					sleep(tiempo_aleatorio(1, 5));
					//printf("[Recepción] Registrando nuevo paciente: %s...\n", &paciente->nombre);
				}

    		}
	}

	free(lista_Espera_Pacientes);
    free(lista_Pacientes);
	exit(0);
}
