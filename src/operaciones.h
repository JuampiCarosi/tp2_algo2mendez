#ifndef __OPERACIONES_H__
#define __OPERACIONES_H__

#include <stdbool.h>
#include <stddef.h>

#include "hash.h"
#include "lista.h"

#define MAX_NOMBRE_ARCHIVO 30
#define MAX_NOMBRE_POKEMON 30
/*
 * Recibe un vector de strings con los nombres de los archivos de cajas a cargar.
 * Devuelve si los nombres de archivos son validos (no contienen puntos antes de la extension ni espacios).
 * En caso de error devuelve false
 */
bool validar_cajas_ingresadas(char *nombres_cajas[], int cantidad_cajas);

/*
 * Recibe un string con el nombre de un archivo.
 * Devuelve el string sin la extension.
 * En caso de error devuelve NULL
 */
char *nombre_archivo_a_caja(const char *nombre_archivo);

/*
 * Carga los archivos de cajas en un hash contenedor de cajas.
 * En caso de error escribe NULL en el hash
 */
void cargar_cajas(hash_t **contenedor_de_cajas, int cantidad_cajas, char *nombres_cajas[]);

/*
 * Recibe un hash contenedor de cajas y un nombre de pokemon.
 * Devuelve una lista con los nombres de las cajas que contienen al pokemon.
 * En caso de error devuelve NULL
 */
lista_t *buscar_cajas_con_pokemon(hash_t *contenedor_de_cajas, char nombre_pokemon[MAX_NOMBRE_POKEMON]);

/*
 * Recibe dos nombres de caja y un archivo de salida.
 * Escribe la caja combinada en el archivo de salida y devuelve el contenedor con
 * la caja nueva agregada.
 * En caso de error devuelve NULL
 */
hash_t *combinar_cajas(char nombre1[MAX_NOMBRE_ARCHIVO], char nombre2[MAX_NOMBRE_ARCHIVO],
                       char nombre3[MAX_NOMBRE_ARCHIVO], hash_t *contenedor_de_cajas);

/*
 * Libera la memoria reservada por el hash contenedor de cajas.
 */
void destruir_contenedor_cajas(hash_t *contenedor_de_cajas);

#endif  // __OPERACIONES_H__