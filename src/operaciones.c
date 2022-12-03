#include "operaciones.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * PRE: el string no es NULL.
 */
char *duplicar_string(const char *string) {
  char *string_duplicado = malloc(sizeof(char) * (strlen(string) + 1));
  if (!string_duplicado) return NULL;
  strcpy(string_duplicado, string);
  return string_duplicado;
}

char *nombre_archivo_a_caja(const char *nombre_archivo) {
  int i = 0;
  char *nombre_caja = malloc(sizeof(char) * MAX_NOMBRE_ARCHIVO);
  if (!nombre_caja) {
    return NULL;
  }

  while (nombre_archivo[i] != '.' && nombre_archivo[i] != '\0') {
    nombre_caja[i] = nombre_archivo[i];
    i++;
  }
  nombre_caja[i] = '\0';
  return nombre_caja;
}

hash_t *combinar_cajas(char nombre1[MAX_NOMBRE_ARCHIVO], char nombre2[MAX_NOMBRE_ARCHIVO],
                       char archivo[MAX_NOMBRE_ARCHIVO], hash_t *contenedor_de_cajas,
                       lista_t *lista_pokemones_indexados) {
  if (!contenedor_de_cajas || !nombre1 || !nombre2 || !archivo || !lista_pokemones_indexados) return NULL;

  char *nombre3 = nombre_archivo_a_caja(archivo);
  if (hash_contiene(contenedor_de_cajas, nombre3)) {
    free(nombre3);
    return NULL;
  }

  caja_t *caja_1 = hash_obtener(contenedor_de_cajas, nombre1);
  caja_t *caja_2 = hash_obtener(contenedor_de_cajas, nombre2);
  if (!caja_1 || !caja_2) {
    free(nombre3);
    return NULL;
  }

  caja_t *caja3 = caja_combinar(caja_1, caja_2);

  if (caja_guardar_archivo(caja3, nombre3) == 0) {
    free(nombre3);
    return NULL;
  }

  hash_t *cajas_combinadas = hash_insertar(contenedor_de_cajas, nombre3, caja3, NULL);
  indexar_nueva_caja(lista_pokemones_indexados, caja3, nombre3);

  free(nombre3);
  return cajas_combinadas;
}

hash_t *cargar_cajas(int cantidad_cajas, char *nombres_cajas[], lista_t *lista_fallidos) {
  if (!nombres_cajas) return NULL;

  if (cantidad_cajas < 1) return NULL;

  hash_t *contenedor_de_cajas = hash_crear((size_t)cantidad_cajas);
  if (!contenedor_de_cajas) return NULL;

  bool hay_error = false;
  int i = 0;

  while (i < cantidad_cajas && !hay_error) {
    caja_t *caja = caja_cargar_archivo(nombres_cajas[i]);
    char *nombre = nombre_archivo_a_caja(nombres_cajas[i]);

    if (!caja)
      hay_error = true;
    else if (!hash_insertar(contenedor_de_cajas, nombre, caja, NULL))
      hay_error = true;

    if (hay_error && lista_fallidos)
      lista_insertar(lista_fallidos, nombre);
    else
      free(nombre);

    i++;
  }

  if (hay_error) {
    hash_destruir(contenedor_de_cajas);
    return NULL;
  }

  return contenedor_de_cajas;
}

typedef struct {
  char *nombre;
  lista_t *cajas;
} estructura_iteradora_cajas_t;

bool buscar_pokemon(void *valor, void *aux) {
  hash_t *hash = valor;
  char *nombre = ((estructura_iteradora_cajas_t *)aux)->nombre;
  lista_t *lista = ((estructura_iteradora_cajas_t *)aux)->cajas;

  char *nombre_caja = hash_obtener(hash, nombre);

  if (nombre_caja) lista_insertar_en_posicion(lista, nombre_caja, 0);

  return true;
}

lista_t *buscar_cajas_con_pokemon(lista_t *lista_pokemones_indexados, char nombre_pokemon[MAX_NOMBRE_POKEMON]) {
  if (!lista_pokemones_indexados || !nombre_pokemon) return NULL;

  lista_t *cajas = lista_crear();
  if (!cajas) return NULL;

  estructura_iteradora_cajas_t estructura = {.nombre = nombre_pokemon, .cajas = cajas};

  lista_con_cada_elemento(lista_pokemones_indexados, buscar_pokemon, &estructura);

  return cajas;
}

bool vector_contiene_string_repetido(char *vector[], char *string, int cantidad_elementos) {
  int i = 0;
  int cantidad_encontrados = 0;
  while (i < cantidad_elementos && cantidad_encontrados < 2) {
    if (strcmp(vector[i], string) == 0) {
      cantidad_encontrados++;
    }
    i++;
  }
  return cantidad_encontrados > 1;
}

bool validar_nombre_archivo(char *nombre_archivo) {
  int cantidad_puntos = 0;
  for (int i = 0; i < strlen(nombre_archivo); i++) {
    if (nombre_archivo[i] == '.') cantidad_puntos++;
    if (nombre_archivo[i] == ' ' || cantidad_puntos > 1) return false;
  }
  return true;
}

bool validar_cajas_ingresadas(char *nombres_cajas[], int cantidad_cajas) {
  if (cantidad_cajas < 1 || !nombres_cajas) return false;
  bool cajas_validas = true;
  int i = 0;
  while (i < cantidad_cajas && cajas_validas) {
    cajas_validas = validar_nombre_archivo(nombres_cajas[i]) &&
                    !vector_contiene_string_repetido(nombres_cajas, nombres_cajas[i], cantidad_cajas);
    i++;
  }

  return cajas_validas;
}

bool indexar_pokemones(const char *clave, void *valor, void *aux) {
  lista_t *lista = aux;
  caja_t *caja = valor;
  hash_t *pokemones = hash_crear((size_t)caja_cantidad(caja));
  for (int i = 0; i < caja_cantidad(caja); i++) {
    char *nombre_caja = duplicar_string(clave);
    pokemon_t *pokemon = caja_obtener_pokemon(caja, i);
    if (hash_contiene(pokemones, pokemon_nombre(pokemon)))
      free(nombre_caja);
    else
      hash_insertar(pokemones, pokemon_nombre(pokemon), nombre_caja, NULL);
  }

  lista_insertar_en_posicion(lista, pokemones, 0);
  return true;
}

lista_t *indexar_cajas(hash_t *contenedor_de_cajas) {
  if (!contenedor_de_cajas) return NULL;

  lista_t *lista_pokemones_indexados = lista_crear();
  if (!lista_pokemones_indexados) return NULL;

  hash_con_cada_clave(contenedor_de_cajas, indexar_pokemones, lista_pokemones_indexados);

  return lista_pokemones_indexados;
}

void indexar_nueva_caja(lista_t *lista_pokemones_indexados, caja_t *caja, const char *nombre_caja) {
  indexar_pokemones(nombre_caja, caja, lista_pokemones_indexados);
}

void destruir_hash_indices(void *valor) {
  hash_t *hash = valor;
  hash_destruir_todo(hash, free);
}

void destruir_lista_pokemones_indexados(lista_t *lista_pokemones_indexados) {
  lista_destruir_todo(lista_pokemones_indexados, destruir_hash_indices);
}

void destructor_caja(void *caja) { caja_destruir(caja); }

void destruir_contenedor_cajas(hash_t *contenedor_de_cajas) {
  hash_destruir_todo(contenedor_de_cajas, destructor_caja);
}