#ifndef recepcion_h
#define recepcion_h

typedef struct recepcion{
  struct Cola *cola_Pacientes;
  int pacientes_Alta;
}Recepcion;

void inicializar_Recepcion(Recepcion *recepcion, struct Cola *cola);
#endif