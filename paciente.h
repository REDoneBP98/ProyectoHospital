#ifndef paciente_h
#define paciente_h

typedef struct paciente{
  int id;
  char nombre[20];
  char apellidos[40];
  int edad;
  char sintomas[100];
  }Paciente;

void inicializar_Paciente(Paciente *p, int id,const char* nombre, const char* apellidos, int edad, char* sintomas);
void mostrar_Paciente(Paciente *p);
#endif