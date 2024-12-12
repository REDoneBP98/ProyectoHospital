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

// Define el tamaño de los nombres de pacientes
#define NOMBRE_PACIENTE_LEN 32

int pacientes_dados_de_alta = 0;
pid_t pid_hospital, pid_recepcion;

// Cola de "Recepción" -> "Exploración"
#define COLA_RECEP_EXPLOR "/recepexplor"
mqd_t cola_recep_explor;

// Semáforo para "Exploración" -> "Diagnóstico"
#define SEM_DIAGNOSTICO "/semdiagnostico"
sem_t* sem_diagnostico;

// Semáforo para "Diagnóstico" -> "Farmacia"
#define SEM_FARMACIA "/semfarmacia"
sem_t* sem_farmacia;

// Señal para "Farmacia" -> "Recepción"
#define SENYAL_FARMACIA_RECEPCION SIGUSR1

int tiempo_aleatorio(int min, int max) {
    return rand() % (max - min + 1) + min;
}

void* exploracion(void* args) {
	printf(" [Exploración] Comienzo mi ejecución...\n");

	while (1) {
		int resultado;
		char paciente[NOMBRE_PACIENTE_LEN];
		printf(" [Exploración] Esperando a un paciente...\n");

		// Se bloqueará hasta recibir un nombre
		// de paciente desde Recepción.
		resultado = mq_receive(cola_recep_explor, paciente, sizeof(paciente), 0);

		if(resultado == -1) {
			fprintf(stderr, " [ERROR][Exploración] Error al leer de la cola. (Codigo: %d)\n", errno);
			exit(1);
		} else {
			// Realizar la exploración
			printf(" [Exploración] Se ha recibido el paciente \"%s\"\n", paciente);

			printf(" [Exploración] Recibido paciente: %s. Realizando exploración...\n", paciente);
			sleep(tiempo_aleatorio(1, 3));
			printf(" [Exploración] Exploración completa. Notificando diagnóstico...\n");

			// Notificar que un paciente pasa a diagnostico (sem + 1)
			sem_post(sem_diagnostico);
		}
	}
}

void* diagnostico(void* args) {
    printf("  [Diagnóstico] Comienzo mi ejecución...\n");
    while (1) {
		// Bloquearse hasta que haya pacientes que diagnosticar.
		sem_wait(sem_diagnostico);

        printf("  [Diagnóstico] Realizando pruebas diagnósticas...\n");
        sleep(tiempo_aleatorio(5, 10));
        printf("  [Diagnóstico] Diagnóstico completado. Notificando farmacia...\n");

		// Mandar el paciente a la farmacia.
		sem_post(sem_farmacia);
    }
}

void* farmacia(void* args) {
    printf("   [Farmacia] Comienzo mi ejecución...\n");
    while (1) {
		// Bloquearse hasta que haya pacientes que diagnosticar.
		sem_wait(sem_farmacia);

        printf("   [Farmacia] Preparando medicación...\n");
        sleep(tiempo_aleatorio(1, 3));
        printf("   [Farmacia] Medicación lista. Enviando señal de alta...\n");

		// Notificar a Recepción de que el paciente debe
		// de ser dado de alta.
		kill(pid_recepcion, SENYAL_FARMACIA_RECEPCION);
    }
}

#define NUM_NOMBRES 50
// Función para obtener un nombre de paciente aleatoriamente,
// para dar algo más de información a los primeros prints.
char* generar_nombre_paciente() {
	// Nota: Este array de nombres ha sido
	// generada con ChatGPT.
	// https://chatgpt.com/share/6759e9f5-8740-800e-a3a8-467ffc634fac

	static char nombres[NUM_NOMBRES][NOMBRE_PACIENTE_LEN] = {
    	"Alejandro", "Beatriz", "Carlos", "Diana", "Eduardo",
    	"Fernanda", "Gabriel", "Héctor", "Isabel", "Javier",
    	"Karla", "Luis", "María", "Natalia", "Oscar",
    	"Patricia", "Quirino", "Rafael", "Sofía", "Tomás",
    	"Úrsula", "Vicente", "Walter", "Ximena", "Yolanda",
    	"Zaira", "Andrea", "Braulio", "Camila", "Daniel",
    	"Esteban", "Francisca", "Guillermo", "Hilda", "Iván",
    	"José", "Laura", "Martín", "Nicolás", "Olga",
    	"Paola", "Ramón", "Salvador", "Teresa", "Ulises",
    	"Valeria", "Wilfredo", "Xiomara", "Yahir", "Zoila"
	};

	return nombres[rand() % NUM_NOMBRES];
}

// Contabiliza la salida del paciente, aumentando
// el contador de pacientes dados de alta en 1.
// Tambien, llama a signal para continuar a la escucha
// de la señal proveniente de Farmacia.
void contabilizar_salida(int sig) {
	pacientes_dados_de_alta++;
	printf("[Recepción] <- Un paciente sale... %d dados de alta en total.\n", pacientes_dados_de_alta);
	signal(SENYAL_FARMACIA_RECEPCION, contabilizar_salida);
}

// Simplemente sale del programa. Es
// usada por el hilo Recepción y Hospital.
void terminar_hilo(int sig) {
	exit(0);
}

int main(int argv, char* argc[]) {
	// Asegurar nombres de pacientes aleatorios
	srand(time(NULL));

	// Asegurarse de que la cola no esta creada (en /dev/mqueue)
	mq_unlink(COLA_RECEP_EXPLOR);

	// Lo mismo para el semaforo (en /dev/shm)
	sem_unlink(SEM_DIAGNOSTICO);
	sem_unlink(SEM_FARMACIA);

	// Crear cola para "Recepción" -> "Exploración"
	struct mq_attr attr;
	attr.mq_flags = 0;
	attr.mq_maxmsg = 10;
	attr.mq_msgsize = 32;
	attr.mq_curmsgs = 0;

	cola_recep_explor = mq_open(COLA_RECEP_EXPLOR, O_RDWR | O_CREAT, 0644, &attr);

	if(cola_recep_explor == (mqd_t)-1) {
		perror("[Recepción] Error al abrir la cola.");
		exit(1);
	}

	// Crear semaforo para diagnostico
	sem_diagnostico = sem_open(SEM_DIAGNOSTICO, O_CREAT, 0644, 0);
	if(sem_diagnostico == SEM_FAILED) {
		perror("Error al crear el semaforo para diagnostico.");
		exit(1);
	}

	// Crear semaforo para farmacia
	sem_farmacia = sem_open(SEM_FARMACIA, O_CREAT, 0644, 0);
	if(sem_farmacia == SEM_FAILED) {
		perror("Error al crear el semaforo para farmacia.");
		exit(1);
	}

	pid_recepcion = fork();

	if (pid_recepcion != 0) {
		pid_hospital = fork();
		if (pid_hospital != 0) {
			printf("[PADRE (%d)] A la espera del fin de hijos.\n", getpid());
			// Evitar que el padre reaccione a CTRL+C.
			signal(SIGINT, SIG_IGN);

			// Proceso padre
			int fin_recepcion, fin_hospital, estado, pid_recibida;
			fin_recepcion = 0;
			fin_hospital = 0;

			// Esperar a la terminación de ambos hijos.
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

			// Limpiar cola y semaforos.
			mq_unlink(COLA_RECEP_EXPLOR);
			sem_unlink(SEM_DIAGNOSTICO);
			sem_unlink(SEM_FARMACIA);
			printf("[PADRE] Limpieza de colas y semáforos realizada.\n");
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
		// Configurar señales de terminacion por SIGINT y
		// contabilizar salida.
		signal(SIGINT, terminar_hilo);
		signal(SENYAL_FARMACIA_RECEPCION, contabilizar_salida);

		while (1) {
			// Se crea un paciente aleatorio
			char* paciente = generar_nombre_paciente();

			sleep(tiempo_aleatorio(1, 10));

			printf("[Recepción] -> Registrando nuevo paciente: %s...\n", paciente);

			int resultado;
			do {
				// Mandamos el nombre del paciente a "Exploración".
				resultado = mq_send(cola_recep_explor, paciente, strlen(paciente) + 1, 0);

				if(resultado == -1) {
					fprintf(stderr, "[ERROR][Recepcion] Error al mandar mensaje en la cola. (Codigo: %d)\n", errno);
					exit(1);
				}
			} while(errno == EAGAIN); 
		}
	}

	return 0;
}
