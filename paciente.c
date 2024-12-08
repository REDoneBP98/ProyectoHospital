#include "paciente.h"
#include <string.h>
#include <stdio.h>
void inicializar_Paciente(Paciente* paciente, int id,const char* nombre, const char* apellidos, int edad, char* sintomas){
    paciente->id=id;
    strcpy(paciente->nombre,nombre);
    strcpy(paciente->apellidos,apellidos);
    paciente->edad = edad;
    strcpy(paciente->sintomas,sintomas);
}

void mostrar_Paciente(Paciente* paciente){
    printf("ID:%i\n",paciente->id);
    printf("Nombre:%s\n",paciente->nombre);
    printf("Apellidos:%s\n",paciente->apellidos);
    printf("Edad:%d\n",paciente->edad);
    printf("Sintomas:%s\n",paciente->sintomas);
}