#include "lista.h"

#include <stddef.h>
#include <stdlib.h>

lista_t *lista_crear()
{
	lista_t *lista = malloc(sizeof(lista_t));
	if (!lista)
		return NULL;
	lista->nodo_inicio = NULL;
	lista->nodo_fin = NULL;
	lista->cantidad = 0;
	return lista;
}

nodo_t *crear_nodo(void *elemento)
{
	nodo_t *nodo = malloc(sizeof(nodo_t));
	if (!nodo)
		return NULL;
	nodo->elemento = elemento;
	nodo->siguiente = NULL;
	return nodo;
}

lista_t *lista_insertar(lista_t *lista, void *elemento)
{
	if (!lista)
		return NULL;

	nodo_t *nodo = crear_nodo(elemento);
	if (!nodo)
		return NULL;

	if (lista->cantidad == 0) {
		lista->nodo_inicio = nodo;
	} else {
		lista->nodo_fin->siguiente = nodo;
	}

	lista->nodo_fin = nodo;
	lista->cantidad++;
	return lista;
}

nodo_t *nodo_en_posicion(lista_t *lista, size_t posicion)
{
	if (!lista || posicion >= lista->cantidad)
		return NULL;

	nodo_t *nodo = lista->nodo_inicio;
	for (int i = 0; i < posicion; i++) {
		nodo = nodo->siguiente;
	}
	return nodo;
}

lista_t *lista_insertar_en_posicion(lista_t *lista, void *elemento,
				    size_t posicion)
{
	if (!lista)
		return NULL;

	if (lista->cantidad == 0 || posicion >= lista->cantidad) {
		return lista_insertar(lista, elemento);
	}

	nodo_t *nodo = crear_nodo(elemento);
	if (!nodo)
		return NULL;

	if (posicion == 0) {
		nodo->siguiente = lista->nodo_inicio;
		lista->nodo_inicio = nodo;
	} else {
		nodo_t *nodo_anterior_a_modificar =
			nodo_en_posicion(lista, posicion - 1);

		if (!nodo_anterior_a_modificar) {
			free(nodo);
			return NULL;
		}
		nodo->siguiente = nodo_anterior_a_modificar->siguiente;
		nodo_anterior_a_modificar->siguiente = nodo;
	}
	lista->cantidad++;
	return lista;
}

void *lista_quitar(lista_t *lista)
{
	if (!lista)
		return NULL;
	if (lista->cantidad == 0)
		return NULL;

	void *elemento = lista->nodo_fin->elemento;
	if (lista->cantidad == 1) {
		free(lista->nodo_fin);
		lista->nodo_inicio = NULL;
		lista->nodo_fin = NULL;
	} else {
		nodo_t *nuevo_fin =
			nodo_en_posicion(lista, lista->cantidad - 2);
		if (!nuevo_fin) {
			free(elemento);
			return NULL;
		}
		free(lista->nodo_fin);
		nuevo_fin->siguiente = NULL;
		lista->nodo_fin = nuevo_fin;
	}
	lista->cantidad--;
	return elemento;
}

void *lista_quitar_de_posicion(lista_t *lista, size_t posicion)
{
	if (!lista || lista->cantidad == 0)
		return NULL;

	if (posicion >= lista->cantidad - 1) {
		return lista_quitar(lista);
	}

	nodo_t *nodo_a_eliminar;

	if (posicion == 0) {
		nodo_a_eliminar = lista->nodo_inicio;
		lista->nodo_inicio = nodo_a_eliminar->siguiente;
	} else {
		nodo_t *nodo_anterior_a_eliminar =
			nodo_en_posicion(lista, posicion - 1);
		if (!nodo_anterior_a_eliminar)
			return NULL;

		nodo_a_eliminar = nodo_anterior_a_eliminar->siguiente;
		nodo_anterior_a_eliminar->siguiente =
			nodo_a_eliminar->siguiente;
	}
	void *elemento = nodo_a_eliminar->elemento;
	free(nodo_a_eliminar);
	lista->cantidad--;
	return elemento;
}

void *lista_elemento_en_posicion(lista_t *lista, size_t posicion)
{
	if (!lista || lista->cantidad <= posicion)
		return NULL;
	nodo_t *nodo = nodo_en_posicion(lista, posicion);
	if (!nodo)
		return NULL;
	return nodo->elemento;
}

void *lista_buscar_elemento(lista_t *lista, int (*comparador)(void *, void *),
			    void *contexto)
{
	if (!lista || !comparador || lista->cantidad == 0)
		return NULL;

	int i = 0;
	bool encontrado = false;
	nodo_t *nodo = lista->nodo_inicio;
	while (i < lista->cantidad && !encontrado) {
		if (comparador(nodo->elemento, contexto) == 0) {
			encontrado = true;
		} else {
			nodo = nodo->siguiente;
			i++;
		}
	}
	if (encontrado) {
		return nodo->elemento;
	} else {
		return NULL;
	}
}

void *lista_primero(lista_t *lista)
{
	if (!lista || lista->cantidad == 0)
		return NULL;
	return lista->nodo_inicio->elemento;
}

void *lista_ultimo(lista_t *lista)
{
	if (!lista || lista->cantidad == 0)
		return NULL;
	return lista->nodo_fin->elemento;
}

bool lista_vacia(lista_t *lista)
{
	if (!lista)
		return true;
	return (lista->cantidad == 0);
}

size_t lista_tamanio(lista_t *lista)
{
	if (!lista)
		return 0;
	return lista->cantidad;
}

void lista_destruir_todo(lista_t *lista, void (*funcion)(void *))
{
	if (!lista)
		return;
	if (lista->cantidad == 0) {
		free(lista);
		return;
	}
	nodo_t *nodo = lista->nodo_inicio;
	nodo_t *nodo_siguiente;

	for (int i = 0; i < lista->cantidad; i++) {
		if (funcion)
			funcion(nodo->elemento);
		nodo_siguiente = nodo->siguiente;
		free(nodo);
		nodo = nodo_siguiente;
	}
	free(lista);
}
void lista_destruir(lista_t *lista)
{
	lista_destruir_todo(lista, NULL);
}

lista_iterador_t *lista_iterador_crear(lista_t *lista)
{
	if (!lista)
		return NULL;
	lista_iterador_t *iterador = malloc(sizeof(lista_iterador_t));
	if (!iterador)
		return NULL;
	iterador->lista = lista;
	iterador->corriente = lista->nodo_inicio;
	return iterador;
}

bool lista_iterador_tiene_siguiente(lista_iterador_t *iterador)
{
	if (!iterador)
		return false;
	return iterador->corriente != NULL;
}

bool lista_iterador_avanzar(lista_iterador_t *iterador)
{
	if (!iterador || !iterador->corriente)
		return false;
	iterador->corriente = iterador->corriente->siguiente;
	return iterador->corriente;
}

void *lista_iterador_elemento_actual(lista_iterador_t *iterador)
{
	if (!iterador || !iterador->corriente)
		return NULL;
	return iterador->corriente->elemento;
}

void lista_iterador_destruir(lista_iterador_t *iterador)
{
	if (!iterador)
		return;
	free(iterador);
}

size_t lista_con_cada_elemento(lista_t *lista, bool (*funcion)(void *, void *),
			       void *contexto)
{
	if (!lista || !funcion || lista->cantidad == 0)
		return 0;

	size_t recorridos = 0;
	nodo_t *nodo = lista->nodo_inicio;
	bool seguir = true;
	while (recorridos < lista->cantidad && seguir) {
		seguir = funcion(nodo->elemento, contexto);
		nodo = nodo->siguiente;
		recorridos++;
	}

	return recorridos;
}
