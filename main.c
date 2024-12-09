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

    typedef struct{
      Paciente* paciente;//El paciente con sus respectivos datos
      int listo_Mensaje;//Indicador de si el mensaje esta listo
    } mensaje_Datos_Paciente;

pthread_mutex_t mutex;//Creamos el mutex para proteger el acceso a la variable compartida
mensaje_Datos_Paciente mensaje;//Creamos el mensaje entre hilos

int pacientes_dados_de_alta = 0;

pid_t pid_hospital, pid_recepcion;
void mandar_Info_Paciente(Paciente* paciente){
    pthread_mutex_lock(&mutex);
    if(!mensaje.listo_Mensaje){
       mensaje.paciente = paciente;
       mensaje.listo_Mensaje = 1;
       printf("Se ha enviado el mensaje!\n");
    }
    pthread_mutex_unlock(&mutex);
}

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

void* diagnostico(void* args) {//Este recibe la informacion de recepción
    printf("[Diagnóstico] Comienzo mi ejecución...\n");
    Paciente* paciente_Rec = NULL;
    while (1) {
        pthread_mutex_lock(&mutex);//Bloqueamos el acceso al mensaje
        if (mensaje.listo_Mensaje) {
            paciente_Rec = mensaje.paciente;
            mensaje.listo_Mensaje = 0;
            printf("Mensaje recibido, paciente con id: %i.\n", paciente_Rec->id);
        }
        pthread_mutex_unlock(&mutex);
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


	pid_recepcion = fork();
    Paciente paciente1;
    inicializar_Paciente(&paciente1, 1, "Mikel", "Oria Uria", 0, 0);

    Cola cola_Recepcion;
    inicializar_Cola(&cola_Recepcion);


	if (pid_recepcion != 0) {
		pid_hospital = fork();
		if (pid_hospital != 0) {
			// Proceso padre



		} else {
			// Proceso hospital
			printf("[Hospital] Comienzo mi ejecución...\n");
            pthread_create(&hilo_Diagnostico, NULL, diagnostico, NULL);//Esto ejecuta el hilo del diagnostico y entra en bucle infinito.


		}
	} else {
		// Proceso recepción, no parece que necesite hilo

    		while (1) {//Aqui va la recepcion!!
                int verificado = 0;
        		char paciente[128];
    			sleep(tiempo_aleatorio(1, 10));
                printf("Detectado paciente, verificando solicitud de paciente...\n");
                // verificado = verificar_Peticion(&cola_Recepcion, &paciente1); //Esta es la linea que da error, no he podido encontrar el porque... Aun.
                if(verificado == 1) {
                  mandar_Info_Paciente(&paciente1);//mandamos la informacion del paciente1 al diagnostico
                }
    		}
	}

	exit(0);
}
