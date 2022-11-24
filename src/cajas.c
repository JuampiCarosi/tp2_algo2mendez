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
  hash_t *indices_de_pokemones;
};

void destructor_pokemon(void *pokemon) { free(pokemon); }

typedef struct lista_pokemones {
  pokemon_t *pokemon;
  struct lista_pokemones *siguiente;
} lista_pokemones_t;

hash_t *hash_insertar_indice_pokemones(hash_t *indices_de_pokemones, pokemon_t *pokemon) {
  const char *vector_indices_de_pokemones[hash_cantidad(pokemones) + 1];
  vector_indices_de_pokemones[0] = NULL;

  hash_con_cada_clave(pokemones, (bool (*)(const char *clave, void *valor, void *aux))hash_a_vector,
                      indices_de_pokemones);

  qsort(vector_indices_de_pokemones, (size_t)tamanio_vector_pokemones(vector_indices_de_pokemones), sizeof(char *),
        (int (*)(const void *, const void *))strcmp);

  for (int i = 0; i < tamanio_vector_pokemones(vector_indices_de_pokemones); i++) {
    char indice[CANTIDAD_DIGITOS_DE_CANTIDAD_DE_POKEMONES];
    sprintf(indice, "%i", i);

    hash_insertar(indices_de_pokemones, indice, (void *)vector_indices_de_pokemones[i], NULL);
  }

  return pokemones;
}

int csv_a_hash_pokemones(FILE *pokemones_f, hash_t *pokemones, hash_t *indices_de_pokemones) {
  pokemones = hash_crear(CANTIDAD_INICIALES_POKEMON);
  indices_de_pokemones = hash_crear(CANTIDAD_INICIALES_POKEMON);

  if (!pokemones) {
    return ERROR;
  }

  char linea[MAX_LINEA];
  bool error = false;
  while (!error && fscanf(pokemones_f, FORMATO_LECTURA, linea) > 0) {
    pokemon_t *pokemon = pokemon_crear_desde_string(linea);
    hash_t *nuevos_pokemones = NULL;
    hash_t *nuevo_indice = NULL;
    if (!pokemon) {
      error = true;
    } else {
      nuevos_pokemones = hash_insertar(pokemones, pokemon_nombre(pokemon), pokemon, NULL);
      nuevo_indice = resultado(indices_de_pokemones, pokemon);
    }

    if (!nuevos_pokemones || !nuevo_indice) {
      error = true;
    } else {
      pokemones = nuevos_pokemones;
      indices_de_pokemones = nuevo_indice;
    }
  }

  if (error && pokemones) {
    hash_destruir_todo(pokemones, destructor_pokemon);
    return ERROR;
  }
  return EXITO;
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

  int resultado = csv_a_hash_pokemones(pokemones_f, caja->pokemones, caja->indices_de_pokemones);
  fclose(pokemones_f);

  if (resultado == ERROR) {
    caja_destruir(caja);
    return NULL;
  }

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

  size_t recorridos = hash_con_cada_clave(
      caja->pokemones, (bool (*)(const char *clave, void *valor, void *aux))guardar_pokemon, pokemones_f);

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

bool hash_insertar_en_mezcla(pokemon_t *pokemon, hash_t *mezcla) {
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

  hash_con_cada_clave(caja1->pokemones, (bool (*)(const char *clave, void *valor, void *aux))hash_insertar_en_mezcla,
                      pokemones_combinados);
  hash_con_cada_clave(caja2->pokemones, (bool (*)(const char *clave, void *valor, void *aux))hash_insertar_en_mezcla,
                      pokemones_combinados);

  hash_t *indices_de_pokemones_combinados = hash_crear(hash_cantidad(pokemones_combinados));
  const char *vector_indices_de_pokemones_combinados[hash_cantidad(pokemones_combinados) + 1];

  hash_con_cada_clave(pokemones_combinados, (bool (*)(const char *clave, void *valor, void *aux))hash_a_vector,
                      indices_de_pokemones_combinados);

  qsort(vector_indices_de_pokemones_combinados,
        (size_t)tamanio_vector_pokemones(vector_indices_de_pokemones_combinados), sizeof(char *),
        (int (*)(const void *, const void *))strcmp);

  caja_combinada->pokemones = pokemones_combinados;
  return caja_combinada;
}

int caja_cantidad(caja_t *caja) {
  if (!caja) return ERROR;
  return (int)hash_cantidad(caja->pokemones);
}

typedef struct estructura_iteradora {
  int recorridos;
  int posicion_requerida;
  pokemon_t *pokemon_obtenido;
} estructura_iteradora_t;

pokemon_t *caja_obtener_pokemon(caja_t *caja, int n) {
  if (!caja || n < 0 || n >= hash_cantidad(caja->pokemones)) return NULL;

  char indice[CANTIDAD_DIGITOS_DE_CANTIDAD_DE_POKEMONES];
  sprintf(indice, "%i", n);

  char *nombre_pokemon = hash_obtener(caja->pokemones, indice);
  if (!nombre_pokemon) return NULL;

  pokemon_t *pokemon = hash_obtener(caja->pokemones, nombre_pokemon);
  return pokemon;
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
