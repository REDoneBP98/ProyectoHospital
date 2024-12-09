#include "paciente.h"
#include <string.h>
#include <stdio.h>
void inicializar_Paciente(Paciente* paciente, int id,const char* nombre, const char* apellidos, int diagnostico, int medicacion){
    paciente->id=id;
    strcpy(paciente->nombre,nombre);
    strcpy(paciente->apellidos,apellidos);
    paciente->diagnostico=diagnostico;
    paciente->medicacion=medicacion;
}

void mostrar_Paciente(Paciente* paciente){
    printf("ID:%i\n",paciente->id);
    printf("Nombre:%s\n",paciente->nombre);
    printf("Apellidos:%s\n",paciente->apellidos);
	printf("Diagnostico:%d\n",paciente->diagnostico);
    printf("Medicacion:%d\n",paciente->medicacion);
}