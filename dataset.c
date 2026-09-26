#include "dataset.h"

#include "geo.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_SIZE 2048
#define FIELD_COUNT 7

static void set_error(char *error, size_t size, const char *format, const char *detail, size_t line) {
    if (line > 0) {
        snprintf(error, size, format, detail, line);
    } else {
        snprintf(error, size, format, detail);
    }
}

static int split_fields(char *line, char *fields[FIELD_COUNT]) {
    size_t index;
    char *cursor = line;

    for (index = 0; index < FIELD_COUNT; index++) {
        char *separator;
        fields[index] = cursor;
        if (index == FIELD_COUNT - 1) {
            return strchr(cursor, ';') == NULL;
        }
        separator = strchr(cursor, ';');
        if (separator == NULL) {
            return 0;
        }
        *separator = '\0';
        cursor = separator + 1;
    }
    return 0;
}

static int copy_field(char *destination, size_t size, const char *source) {
    size_t length = strlen(source);

    if (length >= size) {
        return 0;
    }
    memcpy(destination, source, length + 1);
    return 1;
}

static int parse_coordinate(const char *text, double minimum, double maximum, double *value) {
    char *end;

    errno = 0;
    *value = strtod(text, &end);
    while (*end == ' ' || *end == '\t') {
        end++;
    }
    return errno == 0 && end != text && *end == '\0' && *value >= minimum && *value <= maximum;
}

static int parse_camera(char *line, Camera *camera) {
    char *fields[FIELD_COUNT];

    memset(camera, 0, sizeof(*camera));
    if (!split_fields(line, fields) || fields[0][0] == '\0') {
        return 0;
    }
    if (!copy_field(camera->id, sizeof(camera->id), fields[0]) ||
        !copy_field(camera->location, sizeof(camera->location), fields[1]) ||
        !copy_field(camera->year, sizeof(camera->year), fields[2]) ||
        !copy_field(camera->zone, sizeof(camera->zone), fields[3]) ||
        !copy_field(camera->address, sizeof(camera->address), fields[6])) {
        return 0;
    }
    return parse_coordinate(fields[4], -90.0, 90.0, &camera->latitude) &&
        parse_coordinate(fields[5], -180.0, 180.0, &camera->longitude) &&
        geo_coordinates_valid(camera->latitude, camera->longitude);
}

static int dataset_push(CameraDataset *dataset, const Camera *camera) {
    Camera *grown;

    if (dataset->count == dataset->capacity) {
        size_t capacity = dataset->capacity == 0 ? 32 : dataset->capacity * 2;
        grown = realloc(dataset->items, capacity * sizeof(*grown));
        if (grown == NULL) {
            return 0;
        }
        dataset->items = grown;
        dataset->capacity = capacity;
    }
    dataset->items[dataset->count++] = *camera;
    return 1;
}

int dataset_load(const char *path, CameraDataset *dataset, char *error, size_t error_size) {
    FILE *file;
    char line[LINE_SIZE];
    size_t line_number = 0;

    dataset_free(dataset);
    file = fopen(path, "r");
    if (file == NULL) {
        set_error(error, error_size, "No se puede abrir %s", path, 0);
        return 0;
    }
    while (fgets(line, sizeof(line), file) != NULL) {
        Camera camera;
        size_t length;
        line_number++;
        length = strlen(line);
        if (length > 0 && line[length - 1] != '\n' && !feof(file)) {
            set_error(error, error_size, "Linea demasiado larga en %s:%zu", path, line_number);
            fclose(file);
            dataset_free(dataset);
            return 0;
        }
        line[strcspn(line, "\r\n")] = '\0';
        if (line_number == 1) {
            continue;
        }
        if (line[0] == '\0') {
            continue;
        }
        if (!parse_camera(line, &camera)) {
            set_error(error, error_size, "Registro no valido en %s:%zu", path, line_number);
            fclose(file);
            dataset_free(dataset);
            return 0;
        }
        if (!dataset_push(dataset, &camera)) {
            set_error(error, error_size, "Memoria insuficiente al leer %s", path, 0);
            fclose(file);
            dataset_free(dataset);
            return 0;
        }
    }
    if (ferror(file)) {
        set_error(error, error_size, "Error de lectura en %s", path, 0);
        fclose(file);
        dataset_free(dataset);
        return 0;
    }
    if (fclose(file) != 0) {
        set_error(error, error_size, "Error al cerrar %s", path, 0);
        dataset_free(dataset);
        return 0;
    }
    if (dataset->count == 0) {
        set_error(error, error_size, "No hay registros en %s", path, 0);
        dataset_free(dataset);
        return 0;
    }
    return 1;
}

void dataset_free(CameraDataset *dataset) {
    free(dataset->items);
    dataset->items = NULL;
    dataset->count = 0;
    dataset->capacity = 0;
}
