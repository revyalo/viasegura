#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "geo.h"

#define LG 100
#define MAX_REGISTROS 200
#define SEPARADOR ";"
#define FICHERO_DATOS "data/videovigilancia.txt"
#define FICHERO_SALIDA "camaras.txt"

typedef struct {
    char id[LG];
    char anyo[LG];
    char ubicacion[LG];
    char zona[LG];
    char lat[LG];
    char lon[LG];
    char direccion[LG];
    double latitud;
    double longitud;
} tRegistro;

static const char *ZONAS[] = {
    "LAVAPIES",
    "BALLESTA-GRAN VIA",
    "CENTRO SUR",
    "PLAZA MAYOR",
    "AZCA",
    "MUSEO ARTE PUBLICO"
};

static int cargarDatos(tRegistro registros[], int maxRegistros, const char *ruta);
static void opcionDistancia(const tRegistro registros[], int total);
static void opcionZonas(const tRegistro registros[], int total);
static void listarPorZona(const tRegistro registros[], int total);
static void mostrarEstadisticas(const tRegistro registros[], int total);
static void generarFicheroUbicaciones(const tRegistro registros[], int total);
static int buscarPorId(const tRegistro registros[], int total, const char *id);
static int ubicacionCompartida(const tRegistro registros[], int total, int indice);
static void mostrarRegistro(const tRegistro *registro, FILE *salida);
static void mostrarZonas(void);
static int leerEntero(const char *mensaje, int *valor);
static void leerLinea(const char *mensaje, char destino[], size_t longitud);
static void copiarCampo(char destino[], size_t longitud, const char *origen);
static void limpiarSaltoLinea(char texto[]);

int main(int argc, char *argv[]) {
    tRegistro datos[MAX_REGISTROS];
    const char *rutaDatos = argc > 1 ? argv[1] : FICHERO_DATOS;
    int total = cargarDatos(datos, MAX_REGISTROS, rutaDatos);
    int opcion = -1;

    if (total <= 0) {
        printf("No se han podido cargar registros desde %s\n", rutaDatos);
        return EXIT_FAILURE;
    }

    do {
        printf("\n-------------------VIASEGURA-------------------\n");
        printf("1 - Determinar la distancia entre dos camaras de videovigilancia.\n");
        printf("2 - Mostrar estadisticas de camaras por zonas.\n");
        printf("3 - Generar un fichero con camaras en ubicaciones repetidas.\n");
        printf("0 - Terminar.\n");
        printf("-----------------------------------------------\n");

        if (!leerEntero("Opcion: ", &opcion)) {
            break;
        }

        switch (opcion) {
            case 0:
                break;
            case 1:
                opcionDistancia(datos, total);
                break;
            case 2:
                opcionZonas(datos, total);
                break;
            case 3:
                generarFicheroUbicaciones(datos, total);
                break;
            default:
                printf("Opcion no valida.\n");
                break;
        }
    } while (opcion != 0);

    return EXIT_SUCCESS;
}

static int cargarDatos(tRegistro registros[], int maxRegistros, const char *ruta) {
    FILE *fichero = fopen(ruta, "r");
    char linea[512];
    int total = 0;
    int numeroLinea = 0;

    if (fichero == NULL) {
        printf("Error al abrir el fichero de datos: %s\n", ruta);
        return -1;
    }

    while (fgets(linea, sizeof(linea), fichero) != NULL) {
        tRegistro registro = {0};
        char *token;
        int campo = 0;

        numeroLinea++;
        if (numeroLinea == 1) {
            continue;
        }

        if (total >= maxRegistros) {
            printf("Aviso: se alcanzo el limite de %d registros.\n", maxRegistros);
            break;
        }

        token = strtok(linea, SEPARADOR);
        while (token != NULL && campo < 7) {
            limpiarSaltoLinea(token);

            switch (campo) {
                case 0:
                    copiarCampo(registro.id, sizeof(registro.id), token);
                    break;
                case 1:
                    copiarCampo(registro.ubicacion, sizeof(registro.ubicacion), token);
                    break;
                case 2:
                    copiarCampo(registro.anyo, sizeof(registro.anyo), token);
                    break;
                case 3:
                    copiarCampo(registro.zona, sizeof(registro.zona), token);
                    break;
                case 4:
                    copiarCampo(registro.lat, sizeof(registro.lat), token);
                    registro.latitud = strtod(registro.lat, NULL);
                    break;
                case 5:
                    copiarCampo(registro.lon, sizeof(registro.lon), token);
                    registro.longitud = strtod(registro.lon, NULL);
                    break;
                case 6:
                    copiarCampo(registro.direccion, sizeof(registro.direccion), token);
                    break;
                default:
                    break;
            }

            campo++;
            token = strtok(NULL, SEPARADOR);
        }

        if (registro.id[0] != '\0') {
            registros[total] = registro;
            total++;
        }
    }

    if (fclose(fichero) != 0) {
        printf("Aviso: error al cerrar el fichero de datos.\n");
    }

    return total;
}

static void opcionDistancia(const tRegistro registros[], int total) {
    char id1[LG];
    char id2[LG];
    int indice1;
    int indice2;
    double distancia;

    printf("\nIntroduce dos ID para calcular la distancia geografica.\n");
    leerLinea("ID 1: ", id1, sizeof(id1));
    leerLinea("ID 2: ", id2, sizeof(id2));

    indice1 = buscarPorId(registros, total, id1);
    indice2 = buscarPorId(registros, total, id2);

    if (indice1 == -1 || indice2 == -1) {
        printf("No se ha encontrado uno de los ID introducidos.\n");
        return;
    }

    distancia = distanciaGeografica(
        registros[indice1].latitud,
        registros[indice1].longitud,
        registros[indice2].latitud,
        registros[indice2].longitud
    );

    printf("Distancia entre %s y %s: %.2f km\n", id1, id2, distancia);
}

static void opcionZonas(const tRegistro registros[], int total) {
    int eleccion = -1;

    do {
        printf("\n-------- Zonas --------\n");
        printf("1 - Listar camaras de una zona\n");
        printf("2 - Mostrar numero de camaras por zona\n");
        printf("0 - Volver\n");

        if (!leerEntero("Opcion: ", &eleccion)) {
            return;
        }

        switch (eleccion) {
            case 0:
                break;
            case 1:
                listarPorZona(registros, total);
                break;
            case 2:
                mostrarEstadisticas(registros, total);
                break;
            default:
                printf("Opcion no valida.\n");
                break;
        }
    } while (eleccion != 0);
}

static void listarPorZona(const tRegistro registros[], int total) {
    char zona[LG];
    int encontrados = 0;

    mostrarZonas();
    leerLinea("Zona: ", zona, sizeof(zona));

    for (int i = 0; i < total; i++) {
        if (strcmp(zona, registros[i].zona) == 0) {
            mostrarRegistro(&registros[i], stdout);
            encontrados++;
        }
    }

    if (encontrados == 0) {
        printf("No se han encontrado camaras para esa zona.\n");
    }
}

static void mostrarEstadisticas(const tRegistro registros[], int total) {
    size_t totalZonas = sizeof(ZONAS) / sizeof(ZONAS[0]);

    printf("\nNumero de camaras por zona:\n");
    for (size_t i = 0; i < totalZonas; i++) {
        int contador = 0;

        for (int j = 0; j < total; j++) {
            if (strcmp(ZONAS[i], registros[j].zona) == 0) {
                contador++;
            }
        }

        printf("%-25s %d\n", ZONAS[i], contador);
    }
}

static void generarFicheroUbicaciones(const tRegistro registros[], int total) {
    FILE *fichero = fopen(FICHERO_SALIDA, "w");
    int escritos = 0;

    if (fichero == NULL) {
        printf("Se ha producido un error al crear %s\n", FICHERO_SALIDA);
        return;
    }

    for (int i = 0; i < total; i++) {
        if (ubicacionCompartida(registros, total, i)) {
            mostrarRegistro(&registros[i], fichero);
            escritos++;
        }
    }

    if (fclose(fichero) != 0) {
        printf("Se ha producido un error al cerrar %s\n", FICHERO_SALIDA);
        return;
    }

    printf("Se ha creado %s con %d camaras en ubicaciones repetidas.\n", FICHERO_SALIDA, escritos);
}

static int buscarPorId(const tRegistro registros[], int total, const char *id) {
    for (int i = 0; i < total; i++) {
        if (strcmp(id, registros[i].id) == 0) {
            return i;
        }
    }

    return -1;
}

static int ubicacionCompartida(const tRegistro registros[], int total, int indice) {
    for (int i = 0; i < total; i++) {
        if (i != indice && strcmp(registros[indice].ubicacion, registros[i].ubicacion) == 0) {
            return 1;
        }
    }

    return 0;
}

static void mostrarRegistro(const tRegistro *registro, FILE *salida) {
    fprintf(
        salida,
        "ID: %s\tUbicacion: %s\tAnyo: %s\tZona: %s\tLatitud: %s\tLongitud: %s\tDireccion: %s\n",
        registro->id,
        registro->ubicacion,
        registro->anyo,
        registro->zona,
        registro->lat,
        registro->lon,
        registro->direccion
    );
}

static void mostrarZonas(void) {
    size_t totalZonas = sizeof(ZONAS) / sizeof(ZONAS[0]);

    printf("\nZonas disponibles:\n");
    for (size_t i = 0; i < totalZonas; i++) {
        printf("- %s\n", ZONAS[i]);
    }
}

static int leerEntero(const char *mensaje, int *valor) {
    char linea[32];

    for (;;) {
        char *fin;
        long leido;

        printf("%s", mensaje);
        if (fgets(linea, sizeof(linea), stdin) == NULL) {
            return 0;
        }

        leido = strtol(linea, &fin, 10);
        while (*fin == ' ' || *fin == '\t') {
            fin++;
        }

        if (fin != linea && (*fin == '\n' || *fin == '\0')) {
            *valor = (int)leido;
            return 1;
        }

        printf("Entrada no valida. Introduce un numero.\n");
    }
}

static void leerLinea(const char *mensaje, char destino[], size_t longitud) {
    printf("%s", mensaje);
    if (fgets(destino, longitud, stdin) == NULL) {
        destino[0] = '\0';
        return;
    }

    limpiarSaltoLinea(destino);
}

static void copiarCampo(char destino[], size_t longitud, const char *origen) {
    snprintf(destino, longitud, "%s", origen);
    limpiarSaltoLinea(destino);
}

static void limpiarSaltoLinea(char texto[]) {
    texto[strcspn(texto, "\r\n")] = '\0';
}
