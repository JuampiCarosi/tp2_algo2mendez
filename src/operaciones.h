#ifndef __OPERACIONES_H__
#define __OPERACIONES_H__

#include <stdbool.h>
#include <stddef.h>

#include "hash.h"
#include "lista.h"

#define MAX_NOMBRE_ARCHIVO 100

hash_t *combinar_cajas(char nombre1[MAX_NOMBRE_ARCHIVO], char nombre2[MAX_NOMBRE_ARCHIVO],
                       char nombre3[MAX_NOMBRE_ARCHIVO], hash_t *contenedor_de_cajas);

void cargar_cajas(hash_t **contenedor_de_cajas, int cantidad_cajas, char *nombres_cajas[]);

char *parsear_nombre_caja(char *nombre_archivo);

lista_t *buscar_cajas_con_pokemon(hash_t *contenedor_de_cajas, char nombre_pokemon[MAX_NOMBRE_ARCHIVO]);

bool validar_cajas_ingresadas(char *nombres_cajas[], int cantidad_cajas);

#endif  // __OPERACIONES_H__