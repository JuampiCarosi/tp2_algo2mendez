#include "operaciones.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cajas.h"

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
                       char archivo[MAX_NOMBRE_ARCHIVO], hash_t *contenedor_de_cajas) {
  if (!contenedor_de_cajas || !nombre1 || !nombre2 || !archivo) return NULL;

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
  free(nombre3);
  return cajas_combinadas;
}

void cargar_cajas(hash_t **contenedor_de_cajas, int cantidad_cajas, char *nombres_cajas[]) {
  if (!contenedor_de_cajas || !nombres_cajas) return;

  if (cantidad_cajas < 1) {
    *contenedor_de_cajas = NULL;
    return;
  }

  *contenedor_de_cajas = hash_crear((size_t)cantidad_cajas);
  if (!*contenedor_de_cajas) return;

  for (int i = 0; i < cantidad_cajas; i++) {
    caja_t *caja = caja_cargar_archivo(nombres_cajas[i]);
    char *nombre = nombre_archivo_a_caja(nombres_cajas[i]);

    if (!caja)
      printf("No se pudo cargar la caja '%s'\n", nombre);
    else
      hash_insertar(*contenedor_de_cajas, nombre, caja, NULL);

    free(nombre);
  }
}

typedef struct {
  char *nombre;
  lista_t *cajas;
} estructura_iteradora_cajas_t;

bool buscar_pokemon(const char *clave, void *valor, void *aux) {
  caja_t *caja = valor;
  char *nombre = ((estructura_iteradora_cajas_t *)aux)->nombre;
  lista_t *lista = ((estructura_iteradora_cajas_t *)aux)->cajas;

  bool seguir_recorriendo = true;
  int i = 0;
  while (i < caja_cantidad(caja) && seguir_recorriendo) {
    pokemon_t *pokemon_actual = caja_obtener_pokemon(caja, i);
    if (!pokemon_actual) {
      seguir_recorriendo = false;
    } else if (strcmp(pokemon_nombre(pokemon_actual), nombre) == 0) {
      lista_insertar_en_posicion(lista, (void *)clave, 0);
      seguir_recorriendo = false;
    }
    i++;
  }

  return true;
}

lista_t *buscar_cajas_con_pokemon(hash_t *contenedor_de_cajas, char nombre_pokemon[MAX_NOMBRE_POKEMON]) {
  if (!contenedor_de_cajas || !nombre_pokemon) return NULL;

  lista_t *cajas = lista_crear();
  if (!cajas) return NULL;

  estructura_iteradora_cajas_t estructura = {.nombre = nombre_pokemon, .cajas = cajas};

  hash_con_cada_clave(contenedor_de_cajas, buscar_pokemon, &estructura);

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

void destructor_caja(void *caja) { caja_destruir(caja); }

void destruir_contenedor_cajas(hash_t *contenedor_de_cajas) {
  hash_destruir_todo(contenedor_de_cajas, destructor_caja);
}