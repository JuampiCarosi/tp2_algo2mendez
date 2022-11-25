#include "cajas.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash.h"
#include "pokemon.h"

#define FORMATO_LECTURA "%s\n"
#define FORMATO_ESCRITURA "%s;%i;%i;%i\n"
#define PROPIEDADES_POR_LINEA 4
#define MAX_LINEA 70
#define CANTIDAD_INICIALES_POKEMON 10
#define CANTIDAD_DIGITOS_DE_CANTIDAD_DE_POKEMONES 12

const int ERROR = 0;
const int EXITO = 1;

struct _caja_t {
  hash_t *pokemones;
};

void destructor_pokemon(void *pokemon) { free(pokemon); }

hash_t *csv_a_hash_pokemones(FILE *pokemones_f) {
  hash_t *pokemones = hash_crear(CANTIDAD_INICIALES_POKEMON);

  if (!pokemones) {
    return NULL;
  }

  char linea[MAX_LINEA];
  bool error = false;
  while (!error && fscanf(pokemones_f, FORMATO_LECTURA, linea) > 0) {
    pokemon_t *pokemon = pokemon_crear_desde_string(linea);
    hash_t *nuevos_pokemones = NULL;

    if (!pokemon) {
      error = true;
    } else {
      nuevos_pokemones = hash_insertar(pokemones, pokemon_nombre(pokemon), pokemon, NULL);
    }

    if (!nuevos_pokemones)
      error = true;
    else
      pokemones = nuevos_pokemones;
  }
  if (error && pokemones) {
    hash_destruir_todo(pokemones, destructor_pokemon);
    return NULL;
  }
  return pokemones;
}

caja_t *caja_cargar_archivo(const char *nombre_archivo) {
  if (!nombre_archivo) {
    return NULL;
  }
  FILE *pokemones_f = fopen(nombre_archivo, "r");
  if (!pokemones_f) {
    return NULL;
  }
  caja_t *caja = calloc(1, sizeof(caja_t));
  if (!caja) {
    fclose(pokemones_f);
    return NULL;
  }

  caja->pokemones = csv_a_hash_pokemones(pokemones_f);
  fclose(pokemones_f);

  if (!caja->pokemones) {
    caja_destruir(caja);
    return NULL;
  }

  return caja;
}

void limpiar_archivo(const char *nombre_archivo) {
  FILE *pokemones_f = fopen(nombre_archivo, "w");
  fclose(pokemones_f);
}

bool guardar_pokemon(const char *clave, void *valor, void *aux) {
  FILE *pokemones_f = aux;
  pokemon_t *pokemon = valor;
  int escritos = fprintf(pokemones_f, FORMATO_ESCRITURA, clave, pokemon_nivel(pokemon), pokemon_ataque(pokemon),
                         pokemon_defensa(pokemon));
  return escritos != EOF;
}

int caja_guardar_archivo(caja_t *caja, const char *nombre_archivo) {
  if (!caja || !nombre_archivo) return 0;
  FILE *pokemones_f = fopen(nombre_archivo, "w");
  if (!pokemones_f) return 0;
  if (!caja->pokemones) {
    fclose(pokemones_f);
    return 0;
  }

  size_t recorridos = hash_con_cada_clave(caja->pokemones, guardar_pokemon, pokemones_f);

  fclose(pokemones_f);
  if (recorridos != hash_cantidad(caja->pokemones)) {
    limpiar_archivo(nombre_archivo);
    return 0;
  }

  return (int)hash_cantidad(caja->pokemones);
}

pokemon_t *duplicar_pokemon(pokemon_t *pokemon) {
  char pokemon_string[MAX_LINEA];
  sprintf(pokemon_string, FORMATO_ESCRITURA, pokemon_nombre(pokemon), pokemon_nivel(pokemon), pokemon_ataque(pokemon),
          pokemon_defensa(pokemon));

  pokemon_t *duplicado = pokemon_crear_desde_string(pokemon_string);
  return duplicado;
}

bool hash_insertar_en_mezcla(const char *clave, void *valor, void *aux) {
  pokemon_t *pokemon = (pokemon_t *)valor;
  hash_t *mezcla = (hash_t *)aux;

  if (hash_contiene(mezcla, clave)) return true;

  pokemon_t *duplicado = duplicar_pokemon(pokemon);

  hash_t *hash_insertado = hash_insertar(mezcla, pokemon_nombre(duplicado), duplicado, NULL);

  if (!hash_insertado) {
    free(duplicado);
    return false;
  }

  return true;
}

caja_t *caja_combinar(caja_t *caja1, caja_t *caja2) {
  if (!caja1 || !caja2) return NULL;
  caja_t *caja_combinada = calloc(1, sizeof(caja_t));
  if (!caja_combinada) return NULL;

  if (hash_cantidad(caja1->pokemones) == 0 && hash_cantidad(caja2->pokemones) == 0) return caja_combinada;

  hash_t *pokemones_combinados = hash_crear(hash_cantidad(caja1->pokemones) + hash_cantidad(caja2->pokemones));
  if (!pokemones_combinados) {
    free(caja_combinada);
    return NULL;
  }

  hash_con_cada_clave(caja1->pokemones, hash_insertar_en_mezcla, pokemones_combinados);
  hash_con_cada_clave(caja2->pokemones, hash_insertar_en_mezcla, pokemones_combinados);

  caja_combinada->pokemones = pokemones_combinados;
  return caja_combinada;
}

int caja_cantidad(caja_t *caja) {
  if (!caja) return ERROR;
  return (int)hash_cantidad(caja->pokemones);
}

pokemon_t *caja_obtener_pokemon(caja_t *caja, char *nombre) {
  if (!caja || !nombre) return NULL;

  return hash_obtener(caja->pokemones, nombre);
}

int caja_recorrer(caja_t *caja, bool (*funcion)(pokemon_t *)) {
  if (!caja || !funcion) return ERROR;

  return (int)hash_con_cada_clave(caja->pokemones, (bool (*)(const char *clave, void *valor, void *aux))funcion, NULL);
}

void caja_destruir(caja_t *caja) {
  if (!caja) return;
  if (!caja->pokemones) {
    free(caja);
    return;
  }

  hash_destruir_todo(caja->pokemones, destructor_pokemon);
  free(caja);
}
