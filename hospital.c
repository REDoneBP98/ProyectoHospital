#include <fcntl.h>
#include <errno.h>
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

// Relacionado con la cola entre
// los hilos "Recepción" y "Exploración".
//
// La cola es no bloqueante, para evitar interbloqueo
// al usar semaforos.
#define COLA_RECEP_EXPLOR "/recepexplor"
mqd_t cola_recep_explor;
// El semaforo nos ayuda para la exclusion mutua
// de la cola.
#define SEM_COLA_RECEP_EXPLOR "/semrecepexplor"
sem_t* sem_cola_recep_explor;

int pacientes_dados_de_alta = 0;

pid_t pid_hospital, pid_recepcion;

int tiempo_aleatorio(int min, int max) {
    return rand() % (max - min + 1) + min;
}

void* exploracion(void* args) {
	printf("[Exploración] Comienzo mi ejecución...\n");

	while (1) {
		int resultado;
		char paciente[128];
		printf("[Exploración] Esperando a un paciente...\n");

		sem_wait(sem_cola_recep_explor);
		resultado = mq_receive(cola_recep_explor, paciente, sizeof(paciente), 0);
		sem_post(sem_cola_recep_explor);

		if(resultado == -1) {
			if(errno == EAGAIN) {
				sleep(1);
				continue;
			}

			fprintf(stderr, "[ERROR][Exploración] Error al leer de la cola. (Codigo: %d)\n", errno);
			exit(1);
		} else {
			printf("\t[Exploración] Se ha recibido el paciente \"%s\"\n", paciente);

			printf("[Exploración] Recibido paciente: %s. Realizando exploración...\n", paciente);
			sleep(tiempo_aleatorio(1, 3));
			printf("[Exploración] Exploración completa. Notificando diagnóstico...\n");
		}
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

void terminar_hilo(int sig) {
	exit(0);
}

void main(int argv, char* argc[]) {
	// Asegurarse de que la cola no esta creada (en /dev/mqueue)
	mq_unlink(COLA_RECEP_EXPLOR);
	// Lo mismo para el semaforo (en /dev/shm)
	sem_unlink(SEM_COLA_RECEP_EXPLOR);

	// Crear cola
	struct mq_attr attr;
	attr.mq_flags = 0;
	attr.mq_maxmsg= 1;
	attr.mq_msgsize = 128;
	attr.mq_curmsgs = 0;

	cola_recep_explor = mq_open(COLA_RECEP_EXPLOR, O_RDWR | O_CREAT | O_NONBLOCK, 0644, &attr);

	if(cola_recep_explor == (mqd_t)-1) {
		// ¿Cómo propagamos este error?
		perror("[Recepción] Error al abrir la cola.");
		exit(1);
	}

	// Crear semaforo para la cola
	sem_cola_recep_explor = sem_open(SEM_COLA_RECEP_EXPLOR, O_CREAT, 0644, 1);
	if(sem_cola_recep_explor == SEM_FAILED) {
		perror("Error al crear el semaforo para la cola de recepcion-exploracion.");
		exit(1);
	}

	pid_recepcion = fork();

	if (pid_recepcion != 0) {
		pid_hospital = fork();
		if (pid_hospital != 0) {
			printf("[PADRE (%d)] A la espera del fin de hijos.\n", getpid());

			printf("No hacer nada!\n");
			signal(SIGINT, SIG_IGN);

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
			printf("[PADRE] Fin del proceso padre.\n");

			// Limpiar cola y semaforo.
			mq_unlink(COLA_RECEP_EXPLOR);
			sem_unlink(SEM_COLA_RECEP_EXPLOR);
		} else {
			// Proceso hospital
			printf("[Hospital] Comienzo mi ejecución...\n");
			signal(SIGINT, terminar_hilo);

			// Preconfiguracion de los hilos
			pthread_t id_hilos[3];
			pthread_attr_t attr_hilos[3];
			void* func_hilos[3] = {exploracion, diagnostico, farmacia};

			// Lanzar los hilos "Exploracion", "Diagnostico" y "Farmacia".
			int i;
			for(i = 0; i < 3; i++) {
				pthread_attr_init(&attr_hilos[i]);
				pthread_create(&id_hilos[i], &attr_hilos[i], func_hilos[i], NULL);
			}

			for(i = 0; i < 3; i++) pthread_join(id_hilos[i], NULL);
		}
	} else {
		// Proceso recepción
		printf("[Recepción] Comienzo mi ejecución...\n");
		signal(SIGINT, terminar_hilo);
		while (1) {
			char paciente[128] = "Jose";

			sleep(tiempo_aleatorio(1, 10));

			printf("[Recepción] Registrando nuevo paciente: %s...\n", paciente);

			int resultado;
			do {
				sem_wait(sem_cola_recep_explor);
				resultado = mq_send(cola_recep_explor, paciente, strlen(paciente) + 1, 0);
				sem_post(sem_cola_recep_explor);

				if(resultado == -1) {
					if(errno == EAGAIN) {
						sleep(1);
					} else { // ¿Deberiamos de propagar el error?
						fprintf(stderr, "[ERROR][Recepcion] Error al mandar mensaje en la cola. (Codigo: %d)\n", errno);
						exit(1);
					}
				}
			} while(errno == EAGAIN); 
		}
	}

	exit(0);
}
