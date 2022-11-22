#include "abb.h"

#include <stddef.h>
#include <stdlib.h>

struct vector_t {
	void **array;
	size_t cantidad;
	size_t tamanio;
};

abb_t *abb_crear(abb_comparador comparador)
{
	if (!comparador)
		return NULL;
	abb_t *arbol = malloc(sizeof(abb_t));
	if (!arbol)
		return NULL;
	arbol->nodo_raiz = NULL;
	arbol->comparador = comparador;
	arbol->tamanio = 0;
	return arbol;
}

nodo_abb_t *nodo_crear(void *elemento)
{
	nodo_abb_t *nodo = malloc(sizeof(nodo_abb_t));
	if (!nodo)
		return NULL;
	nodo->elemento = elemento;
	nodo->izquierda = NULL;
	nodo->derecha = NULL;
	return nodo;
}

/*
 * PRE: Nodo a insertar tiene que ser no NULL
 */
nodo_abb_t *abb_insertar_rec(nodo_abb_t *nodo_actual,
			     nodo_abb_t *nodo_a_insertar,
			     abb_comparador comparador)
{
	if (!nodo_actual)
		return nodo_a_insertar;
	if (comparador(nodo_a_insertar->elemento, nodo_actual->elemento) <= 0) {
		nodo_actual->izquierda = abb_insertar_rec(
			nodo_actual->izquierda, nodo_a_insertar, comparador);
	} else {
		nodo_actual->derecha = abb_insertar_rec(
			nodo_actual->derecha, nodo_a_insertar, comparador);
	}
	return nodo_actual;
}

abb_t *abb_insertar(abb_t *arbol, void *elemento)
{
	if (!arbol)
		return NULL;
	nodo_abb_t *nodo = nodo_crear(elemento);
	if (!nodo)
		return NULL;
	arbol->nodo_raiz =
		abb_insertar_rec(arbol->nodo_raiz, nodo, arbol->comparador);
	arbol->tamanio++;
	return arbol;
}

/*
 * PRE: Todos los parametros estan inicializados
 */
void *abb_eliminar_predecesor(nodo_abb_t *raiz, void **predecesor)
{
	if (!raiz->derecha) {
		*predecesor = raiz->elemento;
		nodo_abb_t *raiz_izquierda = raiz->izquierda;
		free(raiz);
		return raiz_izquierda;
	}
	raiz->derecha = abb_eliminar_predecesor(raiz->derecha, predecesor);
	return raiz;
}

/*
 * PRE: elemento, arbol y elemento_eliminado son no NULL
 */
nodo_abb_t *abb_quitar_rec(nodo_abb_t *nodo, void *elemento, abb_t *arbol,
			   void **elemento_eliminado)
{
	if (!nodo)
		return NULL;

	if (arbol->comparador(nodo->elemento, elemento) > 0)
		nodo->izquierda = abb_quitar_rec(nodo->izquierda, elemento,
						 arbol, elemento_eliminado);

	else if (arbol->comparador(nodo->elemento, elemento) < 0)
		nodo->derecha = abb_quitar_rec(nodo->derecha, elemento, arbol,
					       elemento_eliminado);

	else {
		if (!nodo->izquierda || !nodo->derecha) {
			nodo_abb_t *aux = nodo->derecha ? nodo->derecha :
							  nodo->izquierda;
			*elemento_eliminado = nodo->elemento;
			arbol->tamanio--;
			free(nodo);
			return aux;
		}
		void *predecesor = NULL;
		nodo->izquierda =
			abb_eliminar_predecesor(nodo->izquierda, &predecesor);
		*elemento_eliminado = nodo->elemento;
		nodo->elemento = predecesor;
		arbol->tamanio--;
	}
	return nodo;
}

void *abb_quitar(abb_t *arbol, void *elemento)
{
	if (!arbol || arbol->nodo_raiz == NULL)
		return NULL;
	void *elemento_eliminado = NULL;
	arbol->nodo_raiz = abb_quitar_rec(arbol->nodo_raiz, elemento, arbol,
					  &elemento_eliminado);
	return elemento_eliminado;
}

void *abb_buscar_rec(nodo_abb_t *nodo, void *elemento,
		     abb_comparador comparador)
{
	if (!nodo)
		return NULL;

	int comparacion = comparador(nodo->elemento, elemento);
	if (comparacion == 0)
		return nodo->elemento;
	else if (comparacion < 0)
		return abb_buscar_rec(nodo->derecha, elemento, comparador);
	return abb_buscar_rec(nodo->izquierda, elemento, comparador);
}

void *abb_buscar(abb_t *arbol, void *elemento)
{
	if (!arbol)
		return NULL;
	return abb_buscar_rec(arbol->nodo_raiz, elemento, arbol->comparador);
}

bool abb_vacio(abb_t *arbol)
{
	return (!arbol || arbol->tamanio == 0);
}

size_t abb_tamanio(abb_t *arbol)
{
	if (!arbol)
		return 0;
	return arbol->tamanio;
}

void abb_destruir_todo_rec(nodo_abb_t *nodo, void (*destructor)(void *))
{
	if (!nodo)
		return;
	abb_destruir_todo_rec(nodo->izquierda, destructor);
	abb_destruir_todo_rec(nodo->derecha, destructor);
	if (destructor)
		destructor(nodo->elemento);
	free(nodo);
}

void abb_destruir_todo(abb_t *arbol, void (*destructor)(void *))
{
	if (!arbol)
		return;
	abb_destruir_todo_rec(arbol->nodo_raiz, destructor);
	free(arbol);
}

void abb_destruir(abb_t *arbol)
{
	if (!arbol)
		return;
	abb_destruir_todo(arbol, NULL);
}

/*
 * PRE: funcion y seguir_recorriendo tienen que ser no NULL
 */
size_t abb_recorrer_preorden(nodo_abb_t *nodo, bool (*funcion)(void *, void *),
			     void *aux, bool *seguir_recorriendo)
{
	if (!nodo)
		return 0;

	if (!funcion(nodo->elemento, aux)) {
		*seguir_recorriendo = false;
		return 1;
	}

	size_t contador = 1;
	contador += abb_recorrer_preorden(nodo->izquierda, funcion, aux,
					  seguir_recorriendo);
	if (!*seguir_recorriendo)
		return contador;

	contador += abb_recorrer_preorden(nodo->derecha, funcion, aux,
					  seguir_recorriendo);
	return contador;
}

/*
 * PRE: Nodo, funcion y seguir_recorriendo tienen que ser no NULL
 */
size_t abb_recorrer_inorden(nodo_abb_t *nodo, bool (*funcion)(void *, void *),
			    void *aux, bool *seguir_recorriendo)
{
	if (!nodo)
		return 0;

	size_t contador = 0;
	contador += abb_recorrer_inorden(nodo->izquierda, funcion, aux,
					 seguir_recorriendo);
	if (!*seguir_recorriendo)
		return contador;

	contador++;
	if (!funcion(nodo->elemento, aux)) {
		*seguir_recorriendo = false;
		return contador;
	}

	contador += abb_recorrer_inorden(nodo->derecha, funcion, aux,
					 seguir_recorriendo);
	return contador;
}

/*
 * PRE: Nodo, funcion y seguir_recorriendo tienen que ser no NULL
 */
size_t abb_recorrer_postorden(nodo_abb_t *nodo, bool (*funcion)(void *, void *),
			      void *aux, bool *seguir_recorriendo)
{
	if (!nodo)
		return 0;

	size_t contador = 0;
	contador += abb_recorrer_postorden(nodo->izquierda, funcion, aux,
					   seguir_recorriendo);
	if (!*seguir_recorriendo)
		return contador;

	contador += abb_recorrer_postorden(nodo->derecha, funcion, aux,
					   seguir_recorriendo);
	if (!*seguir_recorriendo)
		return contador;

	contador++;
	if (!funcion(nodo->elemento, aux))
		*seguir_recorriendo = false;
	return contador;
}

size_t abb_con_cada_elemento(abb_t *arbol, abb_recorrido recorrido,
			     bool (*funcion)(void *, void *), void *aux)
{
	if (!arbol || !funcion)
		return 0;

	if (recorrido == PREORDEN) {
		bool seguir_recorriendo = true;
		return abb_recorrer_preorden(arbol->nodo_raiz, funcion, aux,
					     &seguir_recorriendo);

	} else if (recorrido == INORDEN) {
		bool seguir_recorriendo = true;
		size_t contador = abb_recorrer_inorden(
			arbol->nodo_raiz, funcion, aux, &seguir_recorriendo);
		return contador;

	} else {
		bool seguir_recorriendo = true;
		return abb_recorrer_postorden(arbol->nodo_raiz, funcion, aux,
					      &seguir_recorriendo);
	}
}

/*
 * PRE: _vector tiene que ser del tipo  "struct vector_t *"
 */
bool push_to_array(void *elemento, void *_vector)
{
	struct vector_t *vector = (struct vector_t *)_vector;
	if (vector->cantidad >= vector->tamanio)
		return false;
	(vector->array)[vector->cantidad] = elemento;
	vector->cantidad++;
	return true;
}

size_t abb_recorrer(abb_t *arbol, abb_recorrido recorrido, void **array,
		    size_t tamanio_array)
{
	if (!array)
		return 0;
	struct vector_t vector = { array, 0, tamanio_array };
	abb_con_cada_elemento(arbol, recorrido, push_to_array, &vector);
	return vector.cantidad;
}
