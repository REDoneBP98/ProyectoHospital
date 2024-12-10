#ifndef paciente_h
#define paciente_h

typedef struct paciente{
  int id;
  char nombre[20];
  char apellidos[40];
  int diagnostico;
  int medicacion;
  }Paciente;

void inicializar_Paciente(Paciente *p, int id,const char* nombre, const char* apellidos, int diagnostico, int medicacion);
void mostrar_Paciente(Paciente *p);
#endif