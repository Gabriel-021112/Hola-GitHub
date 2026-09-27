#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct registro
{
	char nombre[50];
	int  edad;
	char direccion[50];
	char estado_civil[20];
	char pais_residencia[50];
	char comida_favorita[50];
	struct registro *next;
	struct registro *prev;
} Registro;

typedef struct l
{
	Registro *Inicial;
	Registro *Final;
	int len;
} Lista;

Registro *crear(){
	Registro *T;
	T = (Registro *)malloc(sizeof(Registro));
	T->next = NULL;
	T->prev = NULL;
	return T;
}

void crear_lista(Lista *lista){
	Registro *N_inicial;
	Registro *N_final;
	N_inicial = crear();
	N_final = crear();
	lista->Inicial = N_inicial;
	lista->Final = N_final;
	lista->len = 0;

	lista->Inicial->next = N_final;
	lista->Inicial->prev = NULL;

	lista->Final->prev = N_inicial;
	lista->Final->next = NULL;
}

/* Encolar: siempre se agrega al final de la lista (por donde entran) */
int encolar(Lista *lista, char *nombre, int edad, char *direccion,
            char *estado_civil, char *pais_residencia, char *comida_favorita){
	Registro *N;

	N = crear();
	if(N == NULL)
		return 0;

	strcpy(N->nombre, nombre);
	N->edad = edad;
	strcpy(N->direccion, direccion);
	strcpy(N->estado_civil, estado_civil);
	strcpy(N->pais_residencia, pais_residencia);
	strcpy(N->comida_favorita, comida_favorita);

	N->prev = lista->Final->prev;
	N->next = lista->Final;

	lista->Final->prev->next = N;
	lista->Final->prev = N;

	lista->len++;

	return 1;
}

/* Desencolar: siempre se quita del inicio de la lista (por donde salen) */
int desencolar(Lista *lista, Registro *salida){
	Registro *N;

	if(lista->len == 0)
		return 0;

	N = lista->Inicial->next;

	strcpy(salida->nombre, N->nombre);
	salida->edad = N->edad;
	strcpy(salida->direccion, N->direccion);
	strcpy(salida->estado_civil, N->estado_civil);
	strcpy(salida->pais_residencia, N->pais_residencia);
	strcpy(salida->comida_favorita, N->comida_favorita);

	lista->Inicial->next = N->next;
	N->next->prev = lista->Inicial;

	free(N);
	lista->len--;

	return 1;
}

void print_registro(Registro *r){
	printf("\n Nombre           : %s", r->nombre);
	printf("\n Edad              : %d", r->edad);
	printf("\n Direccion         : %s", r->direccion);
	printf("\n Estado civil      : %s", r->estado_civil);
	printf("\n Pais de residencia: %s", r->pais_residencia);
	printf("\n Comida favorita   : %s\n", r->comida_favorita);
}

void print_list(Lista *lista){
	Registro *iter;

	for(iter = lista->Inicial->next; iter != lista->Final; iter = iter->next)
		print_registro(iter);
}

void destruir_lista(Lista *lista){
	Registro *Temp, *T;
	Temp = lista->Inicial;
	while(Temp != NULL){
		T = Temp->next;
		free(Temp);
		Temp = T;
	}
}

int main(){
	Lista lista;
	Registro salida;

	crear_lista(&lista);

	printf(" Tamaño del Registro: %ld bytes\n", sizeof(Registro));

	/* 3 registros de ejemplo */
	encolar(&lista, "Gabriel", 23, "Paramaribo 22", "Casado", "Mexico", "Pizza");
	encolar(&lista, "Eric", 23, "Ticoman", "Soltero", "Mexico", "Tacos al pastor");
	encolar(&lista, "Ana Carla", 21, "Cienfuegos", "Casada", "Cuba", "Hamburguesas");

	printf("\n=== Lista completa (orden de insercion) ===\n");
	print_list(&lista);
	printf("\nTotal de registros: %d\n", lista.len);

	printf("\n=== Sacando registros en orden FIFO ===\n");
	while(desencolar(&lista, &salida)){
		print_registro(&salida);
		printf(" -> Registro sale de la cola. Quedan: %d\n", lista.len);
	}

	destruir_lista(&lista);

	return 0;
}
