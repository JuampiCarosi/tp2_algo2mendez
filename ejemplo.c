#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "src/cajas.h"
#include "src/hash.h"
#include "src/lista.h"
#include "src/pokemon.h"

#define INVENTARIO 'I'
#define CARGAR 'C'
#define COMBINAR 'M'
#define MOSTRAR 'D'
#define BUSCAR 'B'
#define SALIR 'Q'

#define MAX_NOMBRE_ARCHIVO 100

void destructor_caja(void *caja) { caja_destruir(caja); }

char transformar_a_mayuscula(char letra) {
  if (letra >= 'a' && letra <= 'z') {
    return letra - 'a' + 'A';
  }
  return letra;
}

bool mostrar_pokemon(pokemon_t *pokemon) {
  printf("  Nombre: %s\n    Nivel: %i\n    Ataque: %i\n    Defensa: %i\n", pokemon_nombre(pokemon),
         pokemon_nivel(pokemon), pokemon_ataque(pokemon), pokemon_defensa(pokemon));
  return true;
}

char *parsear_nombre_caja(char *nombre_archivo) {
  int i = 0;
  while (nombre_archivo[i] != '.' && nombre_archivo[i] != '\0') {
    i++;
  }
  nombre_archivo[i] = '\0';
  return nombre_archivo;
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

void destruir_contenedor_cajas(hash_t *contenedor_de_cajas) {
  hash_destruir_todo(contenedor_de_cajas, destructor_caja);
}

void mostrar_menu() {
  printf("\nBienvenido al sistema de almacenamiento de cajas de Pokemon\n");
  printf("Por favor seleccione una opcion:\n");
  printf("I. Mostrar inventario\n");
  printf("C. Cargar cajas\n");
  printf("M. Combinar cajas\n");
  printf("D. Mostrar cajas\n");
  printf("B. Buscar cajas\n");
  printf("H. Mas info.\n");
  printf("Q. Salir\n\n");
}

void opcion_ayuda() {
  printf(
      "\nI. Mostrar inventario: Muestra un listado con los nombres de cajas disponibles y la cantidad de pokemon de "
      "cada "
      "caja\n");
  printf(
      "C. Cargar cajas: Pide un nombre de archivo e intenta cargar una caja. Atencion! No puede haber dos cajas con el "
      "mismo nombre\n");
  printf(
      "M. Combinar cajas: Pide dos nombres de cajas y un nombre de archivo. Combina las cajas y las guarda a un "
      "archivo. La nueva caja generada también queda cargada en el sistema. Tener en cuenta que el nombre de la nueva "
      "caja no puede ser repetido.\n");
  printf("D. Mostrar cajas: Pide un nombre de caja y muestra el contenido de la caja por pantalla\n");
  printf(
      "B. Buscar cajas: Pide un nombre de pokemon y muestra la cantidad de cajas que contienen dicho pokemon y la "
      "lista con los nombres de las cajas.\n");
  printf("\nQ. Atras\n");
}

bool mostrar_caja(const char *clave, void *valor, void *aux) {
  printf("\n%i. Nombre: %s\n", *(int *)aux, clave);
  printf("   Cantidad de pokemones: %i\n", caja_cantidad(valor));
  (*(int *)aux)++;
  return true;
}

void opcion_inventario(hash_t *contenedor_de_cajas) {
  int contador = 1;
  hash_con_cada_clave(contenedor_de_cajas, mostrar_caja, &contador);
}

void opcion_cargar(hash_t *contenedor_de_cajas) {
  char nombre_archivo[MAX_NOMBRE_ARCHIVO];
  printf("Ingrese el nombre del archivo: ");
  scanf("%s", nombre_archivo);

  caja_t *caja = caja_cargar_archivo(nombre_archivo);

  if (!caja) {
    printf("Error al cargar el archivo'%s'\n", nombre_archivo);
  } else {
    parsear_nombre_caja(nombre_archivo);
    printf("\nCaja '%s' cargada correctamente!\n", nombre_archivo);
    hash_insertar(contenedor_de_cajas, nombre_archivo, caja, NULL);
  }
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

void opcion_combinar(hash_t *contenedor_de_cajas) {
  char nombre_caja_1[MAX_NOMBRE_ARCHIVO];
  char nombre_caja_2[MAX_NOMBRE_ARCHIVO];
  char nombre_archivo[MAX_NOMBRE_ARCHIVO];

  printf("Ingrese el nombre de la primera caja: ");
  scanf("%s", nombre_caja_1);
  printf("Ingrese el nombre de la segunda caja: ");
  scanf("%s", nombre_caja_2);
  printf("Ingrese el nombre del archivo: ");
  scanf("%s", nombre_archivo);

  hash_t *resultado = combinar_cajas(nombre_caja_1, nombre_caja_2, nombre_archivo, contenedor_de_cajas);
  if (resultado) {
    printf("\nCaja '%s' combinada correctamente!\n", nombre_archivo);
  } else {
    printf(
        "\nError al combinar las cajas, verifique que existan las cajas ingresadas y que no exista la caja a crear\n");
  }
}

void opcion_mostrar(hash_t *contenedor_de_cajas) {
  char nombre_caja[MAX_NOMBRE_ARCHIVO];
  printf("Ingrese el nombre de la caja: ");
  scanf("%s", nombre_caja);

  caja_t *caja = hash_obtener(contenedor_de_cajas, nombre_caja);
  if (!caja) {
    printf("No existe la caja '%s'\n", nombre_caja);
  } else {
    caja_recorrer(caja, mostrar_pokemon);
  }
}

bool buscar_pokemon(const char *clave, void *valor, void *aux) {
  if (strcmp(clave, aux) == 0) lista_insertar((lista_t *)valor, (void *)clave);

  return true;
}

lista_t *buscar_cajas_con_pokemon(hash_t *contenedor_de_cajas, char nombre_pokemon[MAX_NOMBRE_ARCHIVO]) {
  lista_t *cajas = lista_crear();
  if (!cajas) return NULL;

  hash_con_cada_clave(contenedor_de_cajas, buscar_pokemon, nombre_pokemon);

  return cajas;
}

bool mostrar_nombre_caja(void *valor, void *aux) {
  printf("   %s", (char *)valor);
  return true;
}

void opcion_buscar(hash_t *contenedor_de_cajas) {
  char nombre_pokemon[MAX_NOMBRE_ARCHIVO];
  printf("Ingrese el nombre del pokemon: ");
  scanf("%s", nombre_pokemon);

  lista_t *lista = buscar_cajas_con_pokemon(contenedor_de_cajas, nombre_pokemon);

  printf("\nCantidad de cajas que contienen al pokemon '%s': %lu\n", nombre_pokemon, lista_tamanio(lista));

  lista_con_cada_elemento(lista, mostrar_nombre_caja, NULL);

  free(lista);
}

void administrador_de_menu(hash_t *contenedor_de_cajas) {
  char input = ' ';
  while (input != 'Q' && input != 'q') {
    printf("\n");
    mostrar_menu();
    scanf(" %c", &input);
    input = transformar_a_mayuscula(input);
    switch (input) {
      case 'I':
        opcion_inventario(contenedor_de_cajas);
        break;
      case 'C':
        opcion_cargar(contenedor_de_cajas);
        break;
      case 'M':
        opcion_combinar(contenedor_de_cajas);
        break;
      case 'D':
        opcion_mostrar(contenedor_de_cajas);
        break;
      case 'B':
        opcion_buscar(contenedor_de_cajas);
        break;
      case 'H':
        opcion_ayuda();
        break;
      case 'Q':
        printf("\nSaliendo...\n");
        break;
      default:
        printf("Opcion invalida\n");
        break;
    }
  }
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

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Ninguna caja ingresada!\nUso: %s <archivo>\n", argv[0]);
    return 1;
  }

  int cantidad_cajas_ingresadas = argc - 1;
  hash_t *contenedor_de_cajas = NULL;

  if (!validar_cajas_ingresadas(argv + 1, cantidad_cajas_ingresadas)) {
    printf(
        "Las cajas no pueden contener espacios ni puntos, por favor cambie el nombre los archivos e intente "
        "nuevamente");
    return 1;
  }

  cargar_cajas(&contenedor_de_cajas, cantidad_cajas_ingresadas, argv + 1);
  if (!contenedor_de_cajas || hash_cantidad(contenedor_de_cajas) == 0) {
    printf("No se pudo cargar ninguna caja!\nPor favor, ingrese cajas validas e intentelo de nuevo");
    return 1;
  }

  administrador_de_menu(contenedor_de_cajas);

  destruir_contenedor_cajas(contenedor_de_cajas);
  return 0;
}
