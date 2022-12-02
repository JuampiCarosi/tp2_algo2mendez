#include "cajas.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "abb.h"
#include "pokemon.h"

#define FORMATO_LECTURA "%s\n"
#define FORMATO_ESCRITURA "%s;%i;%i;%i\n"
#define PROPIEDADES_POR_LINEA 4
#define MAX_LINEA 70

const int ERROR = 0;
const int EXITO = 1;

struct _caja_t {
  abb_t *pokemones;
};

void destructor_pokemon(void *pokemon) { free(pokemon); }

abb_t *csv_a_abb_pokemones(FILE *pokemones_f) {
  abb_t *pokemones = NULL;
  pokemones = abb_crear((int (*)(void *, void *))strcmp);
  if (!pokemones) {
    return NULL;
  }

  char linea[MAX_LINEA];
  bool error = false;
  while (!error && fscanf(pokemones_f, FORMATO_LECTURA, linea) > 0) {
    pokemon_t *pokemon = pokemon_crear_desde_string(linea);
    abb_t *nuevos_pokemones = NULL;
    if (!pokemon)
      error = true;
    else
      nuevos_pokemones = abb_insertar(pokemones, pokemon);

    if (!nuevos_pokemones)
      error = true;
    else
      pokemones = nuevos_pokemones;
  }
  if (error && pokemones) {
    abb_destruir_todo(pokemones, destructor_pokemon);
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
  caja_t *caja = malloc(sizeof(caja_t));
  if (!caja) {
    fclose(pokemones_f);
    return NULL;
  }

  abb_t *pokemones = NULL;
  pokemones = csv_a_abb_pokemones(pokemones_f);
  fclose(pokemones_f);

  if (!pokemones) {
    free(caja);
    return NULL;
  }

  caja->pokemones = pokemones;

  return caja;
}

void limpiar_archivo(const char *nombre_archivo) {
  FILE *pokemones_f = fopen(nombre_archivo, "w");
  fclose(pokemones_f);
}

bool guardar_pokemon(pokemon_t *pokemon, FILE *pokemones_f) {
  int escritos = fprintf(pokemones_f, FORMATO_ESCRITURA, pokemon_nombre(pokemon), pokemon_nivel(pokemon),
                         pokemon_ataque(pokemon), pokemon_defensa(pokemon));
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

  size_t recorridos =
      abb_con_cada_elemento(caja->pokemones, INORDEN, (bool (*)(void *, void *))guardar_pokemon, pokemones_f);

  fclose(pokemones_f);
  if (recorridos != abb_tamanio(caja->pokemones)) {
    limpiar_archivo(nombre_archivo);
    return 0;
  }

  return (int)abb_tamanio(caja->pokemones);
}

pokemon_t *duplicar_pokemon(pokemon_t *pokemon) {
  char pokemon_string[MAX_LINEA];
  sprintf(pokemon_string, FORMATO_ESCRITURA, pokemon_nombre(pokemon), pokemon_nivel(pokemon), pokemon_ataque(pokemon),
          pokemon_defensa(pokemon));

  pokemon_t *duplicado = pokemon_crear_desde_string(pokemon_string);
  return duplicado;
}

bool abb_insertar_en_mezcla(void *elemento, void *aux) {
  pokemon_t *pokemon = elemento;
  abb_t *mezcla = aux;

  pokemon_t *duplicado = duplicar_pokemon(pokemon);
  abb_t *arbol_insertado = abb_insertar(mezcla, duplicado);
  if (!arbol_insertado) {
    free(duplicado);
    return false;
  }
  return true;
}

caja_t *caja_combinar(caja_t *caja1, caja_t *caja2) {
  if (!caja1 || !caja2) return NULL;
  caja_t *caja_combinada = malloc(sizeof(caja_t));
  if (!caja_combinada) return NULL;

  if (abb_tamanio(caja1->pokemones) == 0 && abb_tamanio(caja2->pokemones) == 0) return caja_combinada;

  abb_t *pokemones_combinados = abb_crear((int (*)(void *, void *))strcmp);
  if (!pokemones_combinados) {
    free(caja_combinada);
    return NULL;
  }

  abb_con_cada_elemento(caja1->pokemones, PREORDEN, abb_insertar_en_mezcla, pokemones_combinados);
  abb_con_cada_elemento(caja2->pokemones, PREORDEN, abb_insertar_en_mezcla, pokemones_combinados);

  caja_combinada->pokemones = pokemones_combinados;
  return caja_combinada;
}

int caja_cantidad(caja_t *caja) {
  if (!caja) return 0;
  return (int)abb_tamanio(caja->pokemones);
}

typedef struct estructura_iteradora {
  int recorridos;
  int posicion_requerida;
  pokemon_t *pokemon_obtenido;
} estructura_iteradora_t;

bool abb_pokemones_obtener_indice(void *elemento, void *aux) {
  pokemon_t *pokemon = elemento;
  estructura_iteradora_t *contexto = aux;
  if (contexto->recorridos == contexto->posicion_requerida) {
    contexto->pokemon_obtenido = pokemon;
    return false;
  }
  return true;
}

pokemon_t *caja_obtener_pokemon(caja_t *caja, int n) {
  if (!caja || n < 0 || n >= abb_tamanio(caja->pokemones)) return NULL;

  estructura_iteradora_t contexto = {0, n, NULL};

  abb_con_cada_elemento(caja->pokemones, INORDEN, abb_pokemones_obtener_indice, &contexto);

  return contexto.pokemon_obtenido;
}

bool funcion_recorrido_caja(void *elemento, void *aux) {
  (*(void (**)(pokemon_t *))aux)((pokemon_t *)elemento);
  return true;
}

int caja_recorrer(caja_t *caja, void (*funcion)(pokemon_t *)) {
  if (!caja || !funcion) return ERROR;

  return (int)abb_con_cada_elemento(caja->pokemones, INORDEN, funcion_recorrido_caja, &funcion);
}

void caja_destruir(caja_t *caja) {
  if (!caja) return;
  if (!caja->pokemones) {
    free(caja);
    return;
  }

  abb_destruir_todo(caja->pokemones, destructor_pokemon);
  free(caja);
}
