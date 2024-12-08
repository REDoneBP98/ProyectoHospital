#include "cola.h"
#include <stdio.h>
#include <stdlib.h>
#include "paciente.h"

void inicializar_Cola(Cola *cola){
    cola->principio = -1;
    cola->final = -1;
}

void meter_A_Cola(Cola *cola, struct Paciente *paciente){
    if(cola->final == 50){
      printf("Lo siento, pero la cola esta llena\n");
    }else{
      if(cola->final == -1){
        cola->final = 0;
      }
      cola->final++;
      cola->pacientes[cola->final] = paciente;//Le metemos en el final de la cola un nuevo paciente.
    }
}

int verificar_Peticion(Cola *cola, struct Paciente *paciente){
  int comprobar = 0;
    for(int i = 0; i < cola->final; i++){
      if(cola->pacientes[i] == paciente){
        printf("Peticion denegada, el paciente ya esta en la cola\n");
        comprobar = 1;
      }
    }
    if(comprobar == 0){
      meter_A_Cola(cola, paciente);
    }
    return comprobar;
}
/*
void sacar_Cola(Cola *cola){
  struct Paciente *paciente = NULL;

  if(cola->final == -1){
    printf("No se pueden sacar pacientes, la cola esta vacia\n");
  }else{
    *paciente = &cola->pacientes[cola->final];//Recogemos el paciente que queremos sacar.
    cola->final--;
    printf("El paciente %s ha salido...\n",paciente->nombre);
  }
}*/