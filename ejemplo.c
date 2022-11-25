#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "src/cajas.h"
#include "src/hash.h"
#include "src/lista.h"
#include "src/operaciones.h"
#include "src/pokemon.h"

#define INVENTARIO 'I'
#define CARGAR 'C'
#define COMBINAR 'M'
#define MOSTRAR 'D'
#define BUSCAR 'B'
#define SALIR 'Q'

#define MAX_NOMBRE_ARCHIVO 100

#define VERDE "\x1b[32;1m"
#define BLANCO "\x1b[30;1m"
#define GRIS "\x1b[37;1m"
#define ROJO "\x1b[31;1m"
#define VIOLETA "\x1b[35;1m"
#define CYAN "\x1b[36;1m"

#define LINEA_SEPARADORA "---------------------------------\n"

void destructor_caja(void *caja) { caja_destruir(caja); }

char transformar_a_mayuscula(char letra) {
  if (letra >= 'a' && letra <= 'z') {
    return letra - 'a' + 'A';
  }
  return letra;
}

bool mostrar_pokemon(const char *clave, void *valor, void *aux) {
  pokemon_t *pokemon = valor;
  printf("  Nombre: %s\n    Nivel: %i\n    Ataque: %i\n    Defensa: %i\n", pokemon_nombre(pokemon),
         pokemon_nivel(pokemon), pokemon_ataque(pokemon), pokemon_defensa(pokemon));
  return true;
}

bool mostrar_caja(const char *clave, void *valor, void *aux) {
  printf("\n%i. Nombre: %s\n", *(int *)aux, clave);
  printf("   Cantidad de pokemones: %i\n", caja_cantidad(valor));
  (*(int *)aux)++;
  return true;
}

bool mostrar_nombre_caja(void *valor, void *aux) {
  printf("-  %s\n", (char *)valor);
  return true;
}

void mostrar_menu() {
  printf("Por favor seleccione una opcion:\n");
  printf(CYAN "I" BLANCO ". Mostrar inventario\n");
  printf(CYAN "C" BLANCO ". Cargar cajas\n");
  printf(CYAN "M" BLANCO ". Combinar cajas\n");
  printf(CYAN "D" BLANCO ". Mostrar cajas\n");
  printf(CYAN "B" BLANCO ". Buscar cajas\n");
  printf(CYAN "H" BLANCO ". Mas info.\n");
  printf(CYAN "Q" BLANCO ". Salir\n\n");
}

void opcion_ayuda() {
  printf(CYAN "\nI" BLANCO ". Mostrar inventario:" GRIS
              " Muestra un listado con los nombres de cajas disponibles y la cantidad de pokemon de "
              "cada "
              "caja\n");
  printf(CYAN "C" BLANCO ". Cargar cajas:" GRIS
              " Pide un nombre de archivo e intenta cargar una caja. Atencion! No puede haber dos cajas con el "
              "mismo nombre\n");
  printf(
      CYAN
      "M" BLANCO ". Combinar cajas:" GRIS
      " Pide dos nombres de cajas y un nombre de archivo. Combina las cajas y las guarda a un "
      "archivo. La nueva caja generada también queda cargada en el sistema. Tener en cuenta que el nombre de la nueva "
      "caja no puede ser repetido.\n");
  printf(CYAN "D" BLANCO ". Mostrar cajas:" GRIS
              " Pide un nombre de caja y muestra el contenido de la caja por pantalla\n");
  printf(CYAN "B" BLANCO ". Buscar cajas:" GRIS
              " Pide un nombre de pokemon y muestra la cantidad de cajas que contienen dicho pokemon y la "
              "lista con los nombres de las cajas.\n");
  printf(CYAN "\nQ" BLANCO ". Atras\n");
}

void opcion_inventario(hash_t *contenedor_de_cajas) {
  int contador = 1;
  hash_con_cada_clave(contenedor_de_cajas, mostrar_caja, &contador);
}

void opcion_cargar(hash_t *contenedor_de_cajas) {
  char nombre_archivo[MAX_NOMBRE_ARCHIVO];
  printf("Ingrese el nombre del archivo: " GRIS);
  scanf("%s", nombre_archivo);
  printf(BLANCO);

  caja_t *caja = caja_cargar_archivo(nombre_archivo);

  if (!caja) {
    printf(ROJO "Error al cargar el archivo'%s'\n" BLANCO, nombre_archivo);
  } else {
    parsear_nombre_caja(nombre_archivo);
    printf("\nCaja '%s' cargada correctamente!\n", nombre_archivo);
    hash_insertar(contenedor_de_cajas, nombre_archivo, caja, NULL);
  }
}

void opcion_combinar(hash_t *contenedor_de_cajas) {
  char nombre_caja_1[MAX_NOMBRE_ARCHIVO];
  char nombre_caja_2[MAX_NOMBRE_ARCHIVO];
  char nombre_archivo[MAX_NOMBRE_ARCHIVO];

  printf("Ingrese el nombre de la primera caja: " GRIS);
  scanf("%s", nombre_caja_1);
  printf(BLANCO);

  printf("Ingrese el nombre de la segunda caja: " GRIS);
  scanf("%s", nombre_caja_2);
  printf(BLANCO);

  printf("Ingrese el nombre del archivo: " GRIS);
  scanf("%s", nombre_archivo);
  printf(BLANCO);

  hash_t *resultado = combinar_cajas(nombre_caja_1, nombre_caja_2, nombre_archivo, contenedor_de_cajas);
  if (resultado) {
    printf("\nCaja '%s' combinada correctamente!\n", nombre_archivo);
  } else {
    printf(ROJO
           "\nError al combinar las cajas, verifique que existan las cajas ingresadas y que no exista la caja a "
           "crear\n" BLANCO);
  }
}

void opcion_mostrar(hash_t *contenedor_de_cajas) {
  char nombre_caja[MAX_NOMBRE_ARCHIVO];
  printf("Ingrese el nombre de la caja: " GRIS);
  scanf("%s", nombre_caja);
  printf(BLANCO);

  caja_t *caja = hash_obtener(contenedor_de_cajas, nombre_caja);
  if (!caja) {
    printf(ROJO "No existe la caja '%s'\n" BLANCO, nombre_caja);
  } else {
    caja_recorrer(caja, (bool (*)(pokemon_t *))mostrar_pokemon);
  }
}

void opcion_buscar(hash_t *contenedor_de_cajas) {
  char nombre_pokemon[MAX_NOMBRE_ARCHIVO];
  printf("Ingrese el nombre del pokemon: " GRIS);
  scanf("%s", nombre_pokemon);
  printf(BLANCO);

  lista_t *lista = buscar_cajas_con_pokemon(contenedor_de_cajas, nombre_pokemon);

  printf("\nCajas que contienen al pokemon '%s':\n", nombre_pokemon);
  lista_con_cada_elemento(lista, mostrar_nombre_caja, NULL);
  printf("\nCantidad: %lu\n", lista_tamanio(lista));

  lista_destruir(lista);
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
        printf(VERDE "Mostrar Inventario\n" LINEA_SEPARADORA BLANCO);
        opcion_inventario(contenedor_de_cajas);
        printf(VERDE LINEA_SEPARADORA BLANCO);

        break;
      case 'C':
        printf(VERDE "Cargar Caja\n" LINEA_SEPARADORA BLANCO);
        opcion_cargar(contenedor_de_cajas);
        printf(VERDE LINEA_SEPARADORA BLANCO);

        break;
      case 'M':
        printf(VERDE "Combinar Cajas\n" LINEA_SEPARADORA BLANCO);
        opcion_combinar(contenedor_de_cajas);
        printf(VERDE LINEA_SEPARADORA BLANCO);

        break;
      case 'D':
        printf(VERDE "Mostrar Caja\n" LINEA_SEPARADORA BLANCO);
        opcion_mostrar(contenedor_de_cajas);
        printf(VERDE LINEA_SEPARADORA BLANCO);

        break;
      case 'B':
        printf(VERDE "Buscar Pokemon\n" LINEA_SEPARADORA BLANCO);
        opcion_buscar(contenedor_de_cajas);
        printf(VERDE LINEA_SEPARADORA BLANCO);

        break;
      case 'H':
        opcion_ayuda();
        break;
      case 'Q':
        printf("\nSaliendo...\n");
        break;
      default:
        printf(ROJO "Opcion invalida\n" BLANCO);
        break;
    }
  }
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf(ROJO "Ninguna caja ingresada!\nUso: %s <archivo>\n", argv[0]);
    return 1;
  }

  int cantidad_cajas_ingresadas = argc - 1;
  hash_t *contenedor_de_cajas = NULL;

  if (!validar_cajas_ingresadas(argv + 1, cantidad_cajas_ingresadas)) {
    printf(ROJO
           "Las cajas no pueden contener espacios ni puntos, por favor cambie el nombre los archivos e intente "
           "nuevamente");
    return 1;
  }

  cargar_cajas(&contenedor_de_cajas, cantidad_cajas_ingresadas, argv + 1);

  if (!contenedor_de_cajas || hash_cantidad(contenedor_de_cajas) == 0) {
    printf(ROJO "No se pudo cargar ninguna caja!\nPor favor, ingrese cajas validas e intentelo de nuevo");
    return 1;
  }

  printf(VIOLETA "\nBienvenido al sistema de almacenamiento de cajas de Pokemon\n" BLANCO);

  administrador_de_menu(contenedor_de_cajas);

  hash_destruir_todo(contenedor_de_cajas, destructor_caja);

  return 0;
}
