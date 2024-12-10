#include "recepcion.h"
#include <stdio.h>

void inicializar_Recepcion(Recepcion *recepcion, struct Cola *cola){
    recepcion->cola_Pacientes = cola;//Registrar un paciente es basicamente encolarlo
    recepcion->pacientes_Alta = 0;
    }

