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

int pacientes_dados_de_alta = 0;

pid_t pid_hospital, pid_recepcion;

int tiempo_aleatorio(int min, int max) {
    return rand() % (max - min + 1) + min;
}

void* exploracion(void* args) {
    printf("[Exploración] Comienzo mi ejecución...\n");
    while (1) {
        char paciente[128];
		printf("[Exploración] Esperando a un paciente...\n");

        printf("[Exploración] Recibido paciente: %s. Realizando exploración...\n", paciente);
        sleep(tiempo_aleatorio(1, 3));
        printf("[Exploración] Exploración completa. Notificando diagnóstico...\n");

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

	pid_recepcion = fork();

	if (pid_recepcion != 0) {
		pid_hospital = fork();
		if (pid_hospital != 0) {
			// Proceso padre
			int fin_recepcion, fin_hospital, estado, pid_recibida;
			fin_recepcion = 0;
			fin_hospital = 0;

			while(!fin_recepcion || !fin_hospital) {
				pid_recibida = wait(&estado);

				if(pid_recibida == pid_hospital) {
					fin_hospital = 1;
				} else if(pid_recibida == pid_recepcion) {
					fin_recepcion = 1;
				}
			}

			// Ambos procesos han finalizado
			printf("[PADRE] Fin del proceso padre.");

		} else {
			// Proceso hospital
			printf("[Hospital] Comienzo mi ejecución...\n");

			// aqui se lanzara una funcion con while 1

		}
	} else {
		// Proceso recepción
		printf("[Recepción] Comienzo mi ejecución...\n");


    		while (1) {
        		char paciente[128];
			// aqui se lanzara una funcion con while 1

				sleep(tiempo_aleatorio(1, 10));

        		printf("[Recepción] Registrando nuevo paciente: %s...\n", paciente);

    		}
	}

	exit(0);
}
