#include "operaciones.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cajas.h"

#define CANT_DIGITOS_INDICE_POKEMONES 100

char *parsear_nombre_caja(const char *nombre_archivo) {
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

  char *nombre3 = parsear_nombre_caja(archivo);
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
    char *nombre_caja_parseado = parsear_nombre_caja(nombres_cajas[i]);

    if (!caja)
      printf("No se pudo cargar la caja '%s'\n", nombre_caja_parseado);
    else
      hash_insertar(*contenedor_de_cajas, nombre_caja_parseado, caja, NULL);

    free(nombre_caja_parseado);
  }
}

typedef struct {
  char *nombre;
  lista_t *cajas;
} estructura_iteradora_cajas_t;

bool buscar_pokemon(void *valor, void *aux) {
  hash_t *caja = valor;
  char *nombre = ((estructura_iteradora_cajas_t *)aux)->nombre;
  lista_t *lista = ((estructura_iteradora_cajas_t *)aux)->cajas;

  if (hash_contiene(caja, nombre)) {
    lista_insertar(lista, (void *)nombre);
  }
  return true;
}

lista_t *buscar_cajas_con_pokemon(lista_t *cajas_con_pokemones_indexados, char *nombre_pokemon) {
  if (!cajas_con_pokemones_indexados || !nombre_pokemon) return NULL;

  lista_t *cajas = lista_crear();
  if (!cajas) return NULL;

  estructura_iteradora_cajas_t estructura = {.nombre = nombre_pokemon, .cajas = cajas};

  lista_con_cada_elemento(cajas_con_pokemones_indexados, buscar_pokemon, &estructura);

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

typedef struct {
  char *nombre_caja;
  hash_t *nombres;
} caja_con_nombre_t;

bool insertar_nombres(const char *clave, void *elemento, void *aux) {
  caja_t *caja = elemento;
  lista_t *lista = aux;

  hash_t *caja_con_pokemones = hash_crear(3);

  if (!caja_con_pokemones) return false;

  for (int i = 0; i < caja_cantidad(caja); i++) {
    pokemon_t *pokemon = caja_obtener_pokemon(caja, i);
    const char *nombre_pokemon = pokemon_nombre(pokemon);

    if (!pokemon || !nombre_pokemon) return false;
    hash_insertar(caja_con_pokemones, nombre_pokemon, NULL, NULL);
  }

  caja_con_nombre_t *caja_con_nombre = malloc(sizeof(caja_con_nombre_t));
  if (!caja_con_nombre) return false;
  caja_con_nombre->nombre_caja = (char *)clave;
  caja_con_nombre->nombres = caja_con_pokemones;

  lista_insertar_en_posicion(lista, caja_con_nombre, 0);
  return true;
}

void destruir_indexacion_pokemones(void *elemento) {
  caja_con_nombre_t *caja_con_nombre = elemento;
  hash_destruir(caja_con_nombre->nombres);
  free(caja_con_nombre->nombre_caja);
  free(caja_con_nombre);
}

size_t indexar_pokemones(hash_t *cajas, lista_t **cajas_con_pokemones_indexados) {
  lista_t *lista = lista_crear();
  if (!lista) return 0;

  *cajas_con_pokemones_indexados = lista;

  return hash_con_cada_clave(cajas, insertar_nombres, *cajas_con_pokemones_indexados);
}

void destructor_caja(void *caja) { caja_destruir(caja); }

void destructor_lista_cajas(void *elemento) { hash_destruir(elemento); }

void destruir_contenedor_cajas(contenedor_cajas_t contenedor_cajas) {
  if (contenedor_cajas.cajas) hash_destruir_todo(contenedor_cajas.cajas, destructor_caja);

  if (contenedor_cajas.cajas_con_pokemones_indexados)
    lista_destruir_todo(contenedor_cajas.cajas_con_pokemones_indexados, destructor_lista_cajas);
}
