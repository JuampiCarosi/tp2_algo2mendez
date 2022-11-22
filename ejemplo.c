#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "src/cajas.h"
#include "src/pokemon.h"

const int ERROR = 0;
const int EXITO = 1;

void mostrar_pokemon(pokemon_t *pokemones) {
  printf("  Nombre: %s\n    Nivel: %i\n    Ataque: %i\n    Defensa: %i\n", pokemon_nombre(pokemones),
         pokemon_nivel(pokemones), pokemon_ataque(pokemones), pokemon_defensa(pokemones));
}

int cargar_cajas(caja_t *cajas[], char *nombre_archivos[], char *argv[], int argc) {
  int i = 1;
  for (i = 1; i < argc; i++) {
    cajas[i - 1] = caja_cargar_archivo(argv[i]);
    strcpy(nombre_archivos[i - 1], argv[i]);

    if (!cajas[i - 1]) {
      printf("Error al cargar la caja %s :(\n", argv[i]);
    }
  }
  if (!cajas[0])
    return EXITO;
  else
    return ERROR;
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Se debe ingresar al menos una caja!\nUso: %s <archivo>\n", argv[0]);
    return EXIT_FAILURE;
  }

  caja_t *cajas[argc - 1];
  char *nombre_archivos[argc - 1];

  if (cargar_cajas(cajas, nombre_archivos, argv, argc) == ERROR) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
