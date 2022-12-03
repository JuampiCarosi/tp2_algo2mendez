#include "hash.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "operaciones.h"

#define UMBRAL_FACTOR_CARGA 0.75

typedef struct par {
  char *clave;
  void *valor;
  struct par *siguiente;
} par_t;

struct hash {
  par_t **pares;
  size_t cantidad_elementos;
  size_t capacidad;
};

size_t jenkins_one_at_a_time_hash(char *clave) {
  size_t hash, i;
  for (hash = i = 0; i < strlen(clave); ++i) {
    hash += clave[i];
    hash += (hash << 10);
    hash ^= (hash >> 6);
  }
  hash += (hash << 3);
  hash ^= (hash >> 11);
  hash += (hash << 15);
  return hash;
}

hash_t *hash_crear(size_t capacidad) {
  if (capacidad <= 3) capacidad = 3;

  hash_t *hash = calloc(1, sizeof(hash_t));
  if (!hash) return NULL;

  hash->capacidad = capacidad;
  hash->pares = calloc(capacidad, sizeof(par_t *));
  if (!hash->pares) {
    free(hash);
    return NULL;
  }
  return hash;
}

/*
 * PRE: el hash no es NULL.
 */
hash_t *rehash(hash_t **_hash) {
  hash_t *hash = *_hash;

  hash_t *hash_aux = hash_crear(hash->capacidad * 2);
  if (!hash_aux) return NULL;

  for (size_t i = 0; i < hash->capacidad; i++) {
    par_t *par_actual = hash->pares[i];

    while (par_actual) {
      hash_t *hash_insertado = hash_insertar(hash_aux, par_actual->clave, par_actual->valor, NULL);

      if (!hash_insertado) {
        hash_destruir(hash_aux);
        return NULL;
      }

      par_t *siguiente_par = par_actual->siguiente;
      free(par_actual->clave);
      free(par_actual);
      par_actual = siguiente_par;
    }
  }

  free(hash->pares);
  hash->pares = hash_aux->pares;
  hash->capacidad = hash_aux->capacidad;
  hash->cantidad_elementos = hash_aux->cantidad_elementos;
  free(hash_aux);

  return *_hash;
}

par_t *crear_par(const char *clave, void *valor) {
  if (!clave) return NULL;

  par_t *par = malloc(sizeof(par_t));
  if (!par) return NULL;

  par->clave = duplicar_string(clave);
  if (!par->clave) {
    free(par);
    return NULL;
  }

  par->valor = valor;
  par->siguiente = NULL;
  return par;
}

/*
 * PRE: clave, cantidad_elementos y hay_error no son NULL.
 */
par_t *insertar_par_en_posicion(par_t *par, const char *clave, void *elemento, void **anterior,
                                size_t *cantidad_elementos, bool *hay_error) {
  if (!par) {
    par = crear_par(clave, elemento);
    if (!par) {
      *hay_error = true;
      return NULL;
    }
    (*cantidad_elementos)++;
    return par;
  }

  if (strcmp(par->clave, clave) == 0) {
    if (anterior) *anterior = par->valor;
    par->valor = elemento;
    return par;
  }

  par->siguiente = insertar_par_en_posicion(par->siguiente, clave, elemento, anterior, cantidad_elementos, hay_error);
  return par;
}

hash_t *hash_insertar(hash_t *hash, const char *clave, void *elemento, void **anterior) {
  if (!hash || !clave) return NULL;
  if (anterior) *anterior = NULL;

  if ((double)(hash->cantidad_elementos + 1) / (double)hash->capacidad >= UMBRAL_FACTOR_CARGA) {
    hash_t *hash_rehasheado = rehash(&hash);
    if (!hash_rehasheado) return NULL;
  }

  bool hay_error = false;
  size_t indice = jenkins_one_at_a_time_hash((char *)clave) % hash->capacidad;
  hash->pares[indice] =
      insertar_par_en_posicion(hash->pares[indice], clave, elemento, anterior, &(hash->cantidad_elementos), &hay_error);

  if (hay_error) return NULL;

  return hash;
}

/*
 * PRE: clave, eliminado y se_elimino_un_elemento no son NULL.
 */
void *quitar_nodo(par_t *par, const char *clave, void **eliminado, bool *se_elimino_un_elemento) {
  if (!par) return NULL;

  if (strcmp(par->clave, clave) == 0) {
    *eliminado = par->valor;
    par_t *siguiente = par->siguiente;
    free(par->clave);
    free(par);
    *se_elimino_un_elemento = true;
    return siguiente;
  }
  par->siguiente = quitar_nodo(par->siguiente, clave, eliminado, se_elimino_un_elemento);
  return par;
}

void *hash_quitar(hash_t *hash, const char *clave) {
  if (!hash || !clave) return NULL;

  size_t indice = jenkins_one_at_a_time_hash((char *)clave) % hash->capacidad;
  void *eliminado = NULL;
  bool se_elimino_un_elemento = false;

  hash->pares[indice] = quitar_nodo(hash->pares[indice], clave, &eliminado, &se_elimino_un_elemento);
  if (se_elimino_un_elemento) hash->cantidad_elementos--;
  return eliminado;
}

void *hash_obtener(hash_t *hash, const char *clave) {
  if (!hash || !clave) return NULL;

  size_t indice = jenkins_one_at_a_time_hash((char *)clave) % hash->capacidad;
  par_t *par_actual = hash->pares[indice];

  while (par_actual) {
    if (strcmp(par_actual->clave, clave) == 0) return par_actual->valor;
    par_actual = par_actual->siguiente;
  }
  return NULL;
}

bool esta_clave_en_posicion(par_t *par, const char *clave) {
  if (!par) return false;
  if (strcmp(par->clave, clave) == 0) return true;
  return esta_clave_en_posicion(par->siguiente, clave);
}

bool hash_contiene(hash_t *hash, const char *clave) {
  if (!hash || !clave) return false;

  size_t indice = jenkins_one_at_a_time_hash((char *)clave) % hash->capacidad;
  return esta_clave_en_posicion(hash->pares[indice], clave);
}

size_t hash_cantidad(hash_t *hash) {
  if (!hash) return 0;
  return hash->cantidad_elementos;
}

void destruir_pares_en_posicion(par_t *par, void (*destructor)(void *)) {
  if (!par) return;
  destruir_pares_en_posicion(par->siguiente, destructor);
  if (destructor) destructor(par->valor);
  free(par->clave);
  free(par);
}

void hash_destruir_todo(hash_t *hash, void (*destructor)(void *)) {
  if (!hash) return;

  for (size_t i = 0; i < hash->capacidad; i++) {
    destruir_pares_en_posicion(hash->pares[i], destructor);
  }
  free(hash->pares);
  free(hash);
}

void hash_destruir(hash_t *hash) { hash_destruir_todo(hash, NULL); }

/*
 * PRE: contador y f no son NULL.
 */
bool recorrer_posicion(par_t *par, size_t *contador, bool (*f)(const char *clave, void *valor, void *aux), void *aux) {
  if (!par) return true;

  (*contador)++;

  if (f(par->clave, par->valor, aux)) {
    return recorrer_posicion(par->siguiente, contador, f, aux);
  } else
    return false;
}

size_t hash_con_cada_clave(hash_t *hash, bool (*f)(const char *clave, void *valor, void *aux), void *aux) {
  if (!hash || !f) return 0;

  size_t elementos_recorridos = 0;
  size_t i = 0;
  bool seguir_recorriendo = true;
  par_t **pares = hash->pares;

  while (i < hash->capacidad && seguir_recorriendo) {
    if (pares[i]) {
      seguir_recorriendo = recorrer_posicion(pares[i], &elementos_recorridos, f, aux);
    }

    i++;
  }
  return elementos_recorridos;
}
