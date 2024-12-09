#ifndef cola_h
#define cola_h

    typedef struct cola{
      struct Paciente *pacientes[50];
      int principio;
      int final;
    }Cola;

    void inicializar(Cola *cola);
    void meter_A_Cola(Cola *cola, struct Paciente *paciente);
    //void sacar_Cola(Cola *cola);

#endif