#include "operaciones.h"

#include <stdio.h>
#include <string.h>

#include "cajas.h"

char *parsear_nombre_caja(char *nombre_archivo) {
  int i = 0;
  while (nombre_archivo[i] != '.' && nombre_archivo[i] != '\0') {
    i++;
  }
  nombre_archivo[i] = '\0';
  return nombre_archivo;
}

hash_t *combinar_cajas(char nombre1[MAX_NOMBRE_ARCHIVO], char nombre2[MAX_NOMBRE_ARCHIVO],
                       char nombre3[MAX_NOMBRE_ARCHIVO], hash_t *contenedor_de_cajas) {
  if (hash_contiene(contenedor_de_cajas, nombre3)) return NULL;

  caja_t *caja_1 = hash_obtener(contenedor_de_cajas, nombre1);
  caja_t *caja_2 = hash_obtener(contenedor_de_cajas, nombre2);
  if (!caja_1 || !caja_2) return NULL;

  caja_t *caja3 = caja_combinar(caja_1, caja_2);

  if (caja_guardar_archivo(caja3, nombre3) == 0) return NULL;

  parsear_nombre_caja(nombre3);
  return hash_insertar(contenedor_de_cajas, nombre3, caja3, NULL);
}

void cargar_cajas(hash_t **contenedor_de_cajas, int cantidad_cajas, char *nombres_cajas[]) {
  *contenedor_de_cajas = hash_crear((size_t)cantidad_cajas);
  if (!*contenedor_de_cajas) return;

  for (int i = 0; i < cantidad_cajas; i++) {
    caja_t *caja = caja_cargar_archivo(nombres_cajas[i]);
    parsear_nombre_caja(nombres_cajas[i]);

    if (!caja)
      printf("No se pudo cargar la caja '%s'\n", nombres_cajas[i]);
    else
      hash_insertar(*contenedor_de_cajas, nombres_cajas[i], caja, NULL);
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

  if (caja_obtener_pokemon(caja, nombre)) {
    lista_insertar(lista, (void *)clave);
  }
  return true;
}

lista_t *buscar_cajas_con_pokemon(hash_t *contenedor_de_cajas, char nombre_pokemon[MAX_NOMBRE_ARCHIVO]) {
  lista_t *cajas = lista_crear();
  if (!cajas) return NULL;

  estructura_iteradora_cajas_t estructura = {.nombre = nombre_pokemon, .cajas = cajas};

  hash_con_cada_clave(contenedor_de_cajas, buscar_pokemon, &estructura);

  return cajas;
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
  bool cajas_validas = true;
  int i = 0;
  while (i < cantidad_cajas && cajas_validas) {
    cajas_validas = validar_nombre_archivo(nombres_cajas[i]);
    i++;
  }

  return cajas_validas;
}