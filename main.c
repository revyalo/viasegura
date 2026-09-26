#include "csv.h"
#include "dataset.h"
#include "geo.h"
#include "query.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_DATA_FILE "data/videovigilancia.txt"
#define DEFAULT_REPEATED_FILE "camaras.csv"

typedef struct {
    const char *data_path;
    const char *zone;
    const char *csv_path;
    double latitude;
    double longitude;
    double radius;
    int has_latitude;
    int has_longitude;
    int has_radius;
    int show_help;
} CliOptions;

static void print_usage(const char *program) {
    printf(
        "Uso:\n"
        "  %s [fichero]                         Modo interactivo\n"
        "  %s [--data fichero] --lat N --lon N --radius KM [--zone Z] [--csv salida]\n\n"
        "Opciones:\n"
        "  --data RUTA    Fichero separado por ';' (por defecto %s)\n"
        "  --lat N        Latitud de origen, entre -90 y 90\n"
        "  --lon N        Longitud de origen, entre -180 y 180\n"
        "  --radius KM    Radio de busqueda no negativo\n"
        "  --zone TEXTO   Filtro exacto de zona\n"
        "  --csv RUTA     Exporta los resultados a CSV\n"
        "  -h, --help     Muestra esta ayuda\n",
        program,
        program,
        DEFAULT_DATA_FILE
    );
}

static int parse_double(const char *text, double *value) {
    char *end;

    errno = 0;
    *value = strtod(text, &end);
    while (*end == ' ' || *end == '\t') {
        end++;
    }
    return errno == 0 && end != text && *end == '\0';
}

static int take_value(int argc, char *argv[], int *index, const char **value) {
    if (*index + 1 >= argc) {
        fprintf(stderr, "Falta el valor de %s\n", argv[*index]);
        return 0;
    }
    *value = argv[++(*index)];
    return 1;
}

static int parse_options(int argc, char *argv[], CliOptions *options) {
    int index;

    memset(options, 0, sizeof(*options));
    options->data_path = DEFAULT_DATA_FILE;
    for (index = 1; index < argc; index++) {
        const char *value;
        if (strcmp(argv[index], "-h") == 0 || strcmp(argv[index], "--help") == 0) {
            options->show_help = 1;
        } else if (strcmp(argv[index], "--data") == 0) {
            if (!take_value(argc, argv, &index, &options->data_path)) {
                return 0;
            }
        } else if (strcmp(argv[index], "--lat") == 0) {
            if (!take_value(argc, argv, &index, &value) || !parse_double(value, &options->latitude)) {
                fprintf(stderr, "Latitud no valida\n");
                return 0;
            }
            options->has_latitude = 1;
        } else if (strcmp(argv[index], "--lon") == 0) {
            if (!take_value(argc, argv, &index, &value) || !parse_double(value, &options->longitude)) {
                fprintf(stderr, "Longitud no valida\n");
                return 0;
            }
            options->has_longitude = 1;
        } else if (strcmp(argv[index], "--radius") == 0) {
            if (!take_value(argc, argv, &index, &value) || !parse_double(value, &options->radius)) {
                fprintf(stderr, "Radio no valido\n");
                return 0;
            }
            options->has_radius = 1;
        } else if (strcmp(argv[index], "--zone") == 0) {
            if (!take_value(argc, argv, &index, &options->zone)) {
                return 0;
            }
        } else if (strcmp(argv[index], "--csv") == 0) {
            if (!take_value(argc, argv, &index, &options->csv_path)) {
                return 0;
            }
        } else if (argv[index][0] != '-' && argc == 2) {
            options->data_path = argv[index];
        } else {
            fprintf(stderr, "Opcion no reconocida: %s\n", argv[index]);
            return 0;
        }
    }
    return 1;
}

static int has_query(const CliOptions *options) {
    return options->has_latitude || options->has_longitude || options->has_radius ||
        options->zone != NULL || options->csv_path != NULL;
}

static int validate_query(const CliOptions *options) {
    if (!options->has_latitude || !options->has_longitude || !options->has_radius) {
        fprintf(stderr, "--lat, --lon y --radius deben indicarse juntos\n");
        return 0;
    }
    if (!geo_coordinates_valid(options->latitude, options->longitude)) {
        fprintf(stderr, "Coordenadas fuera de rango\n");
        return 0;
    }
    if (options->radius < 0.0) {
        fprintf(stderr, "El radio no puede ser negativo\n");
        return 0;
    }
    return 1;
}

static int find_by_id(const CameraDataset *dataset, const char *id) {
    size_t index;

    for (index = 0; index < dataset->count; index++) {
        if (strcmp(dataset->items[index].id, id) == 0) {
            return (int)index;
        }
    }
    return -1;
}

static void read_line(const char *prompt, char *destination, size_t size) {
    int character;

    printf("%s", prompt);
    if (fgets(destination, (int)size, stdin) == NULL) {
        destination[0] = '\0';
        return;
    }
    if (strchr(destination, '\n') == NULL && !feof(stdin)) {
        while ((character = getchar()) != '\n' && character != EOF) {
        }
    }
    destination[strcspn(destination, "\r\n")] = '\0';
}

static int read_menu_option(void) {
    char input[32];
    char *end;
    long value;

    read_line("Opcion: ", input, sizeof(input));
    errno = 0;
    value = strtol(input, &end, 10);
    return errno == 0 && end != input && *end == '\0' ? (int)value : -1;
}

static void print_camera(const Camera *camera) {
    printf("%-8s %-22s %-18s %10.5f %11.5f  %s\n",
        camera->id, camera->location, camera->zone,
        camera->latitude, camera->longitude, camera->address);
}

static void interactive_distance(const CameraDataset *dataset) {
    char first_id[CAMERA_ID_SIZE];
    char second_id[CAMERA_ID_SIZE];
    int first;
    int second;

    read_line("ID 1: ", first_id, sizeof(first_id));
    read_line("ID 2: ", second_id, sizeof(second_id));
    first = find_by_id(dataset, first_id);
    second = find_by_id(dataset, second_id);
    if (first < 0 || second < 0) {
        puts("No se ha encontrado uno de los ID introducidos.");
        return;
    }
    printf("Distancia entre %s y %s: %.3f km\n", first_id, second_id,
        distanciaGeografica(
            dataset->items[first].latitude,
            dataset->items[first].longitude,
            dataset->items[second].latitude,
            dataset->items[second].longitude
        ));
}

static void interactive_zone(const CameraDataset *dataset) {
    char zone[CAMERA_ZONE_SIZE];
    size_t index;
    size_t found = 0;

    read_line("Zona (texto exacto): ", zone, sizeof(zone));
    for (index = 0; index < dataset->count; index++) {
        if (strcmp(dataset->items[index].zone, zone) == 0) {
            print_camera(&dataset->items[index]);
            found++;
        }
    }
    printf("%zu camara(s) encontradas.\n", found);
}

static void interactive_stats(const CameraDataset *dataset) {
    size_t index;

    for (index = 0; index < dataset->count; index++) {
        size_t previous;
        size_t count = 0;
        for (previous = 0; previous < index; previous++) {
            if (strcmp(dataset->items[previous].zone, dataset->items[index].zone) == 0) {
                break;
            }
        }
        if (previous != index) {
            continue;
        }
        for (previous = index; previous < dataset->count; previous++) {
            if (strcmp(dataset->items[previous].zone, dataset->items[index].zone) == 0) {
                count++;
            }
        }
        printf("%-25s %zu\n", dataset->items[index].zone, count);
    }
}

static void interactive_repeated(const CameraDataset *dataset) {
    char error[256];
    size_t written = 0;

    if (!csv_export_repeated_locations(DEFAULT_REPEATED_FILE, dataset, &written, error, sizeof(error))) {
        fprintf(stderr, "%s\n", error);
        return;
    }
    printf("Se ha creado %s con %zu camara(s).\n", DEFAULT_REPEATED_FILE, written);
}

static void run_interactive(const CameraDataset *dataset) {
    int option;

    do {
        puts("\n------------------- VIASEGURA -------------------");
        puts("1 - Distancia entre dos camaras");
        puts("2 - Listar camaras por zona");
        puts("3 - Estadisticas por zona");
        puts("4 - Exportar ubicaciones repetidas a CSV");
        puts("0 - Terminar");
        option = read_menu_option();
        switch (option) {
            case 0: break;
            case 1: interactive_distance(dataset); break;
            case 2: interactive_zone(dataset); break;
            case 3: interactive_stats(dataset); break;
            case 4: interactive_repeated(dataset); break;
            default: puts("Opcion no valida."); break;
        }
    } while (option != 0 && !feof(stdin));
}

static int run_query(const CameraDataset *dataset, const CliOptions *options) {
    CameraDistance *results = NULL;
    size_t count = 0;
    size_t index;
    char error[256];

    if (!query_nearby(dataset, options->latitude, options->longitude, options->radius,
        options->zone, &results, &count, error, sizeof(error))) {
        fprintf(stderr, "%s\n", error);
        return EXIT_FAILURE;
    }
    printf("%-10s %-8s %-18s %s\n", "KM", "ID", "ZONA", "DIRECCION");
    for (index = 0; index < count; index++) {
        printf("%-10.3f %-8s %-18s %s\n", results[index].distance_km,
            results[index].camera->id, results[index].camera->zone,
            results[index].camera->address);
    }
    printf("%zu camara(s) dentro de %.3f km.\n", count, options->radius);

    if (options->csv_path != NULL &&
        !csv_export_nearby(options->csv_path, results, count, error, sizeof(error))) {
        fprintf(stderr, "%s\n", error);
        free(results);
        return EXIT_FAILURE;
    }
    if (options->csv_path != NULL) {
        printf("Resultados exportados a %s\n", options->csv_path);
    }
    free(results);
    return EXIT_SUCCESS;
}

int main(int argc, char *argv[]) {
    CliOptions options;
    CameraDataset dataset = {0};
    char error[256];
    int status = EXIT_SUCCESS;

    if (!parse_options(argc, argv, &options)) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }
    if (options.show_help) {
        print_usage(argv[0]);
        return EXIT_SUCCESS;
    }
    if (has_query(&options) && !validate_query(&options)) {
        return EXIT_FAILURE;
    }
    if (!dataset_load(options.data_path, &dataset, error, sizeof(error))) {
        fprintf(stderr, "%s\n", error);
        return EXIT_FAILURE;
    }

    if (has_query(&options)) {
        status = run_query(&dataset, &options);
    } else {
        run_interactive(&dataset);
    }
    dataset_free(&dataset);
    return status;
}
