#include "csv.h"

#include <stdio.h>

static void write_field(FILE *file, const char *value) {
    const char *cursor;

    fputc('"', file);
    for (cursor = value; *cursor != '\0'; cursor++) {
        if (*cursor == '"') {
            fputc('"', file);
        }
        fputc(*cursor, file);
    }
    fputc('"', file);
}

static void write_camera(FILE *file, const Camera *camera) {
    write_field(file, camera->id);
    fputc(',', file);
    write_field(file, camera->location);
    fputc(',', file);
    write_field(file, camera->year);
    fputc(',', file);
    write_field(file, camera->zone);
    fprintf(file, ",%.8f,%.8f,", camera->latitude, camera->longitude);
    write_field(file, camera->address);
}

static FILE *open_csv(const char *path, char *error, size_t error_size) {
    FILE *file = fopen(path, "w");

    if (file == NULL) {
        snprintf(error, error_size, "No se puede crear %s", path);
    }
    return file;
}

static int close_csv(FILE *file, const char *path, char *error, size_t error_size) {
    if (ferror(file)) {
        snprintf(error, error_size, "Error al escribir %s", path);
        fclose(file);
        return 0;
    }
    if (fclose(file) != 0) {
        snprintf(error, error_size, "Error al cerrar %s", path);
        return 0;
    }
    return 1;
}

int csv_export_nearby(
    const char *path,
    const CameraDistance *results,
    size_t count,
    char *error,
    size_t error_size
) {
    FILE *file = open_csv(path, error, error_size);
    size_t index;

    if (file == NULL) {
        return 0;
    }
    fputs("distance_km,id,location,year,zone,latitude,longitude,address\n", file);
    for (index = 0; index < count; index++) {
        fprintf(file, "%.6f,", results[index].distance_km);
        write_camera(file, results[index].camera);
        fputc('\n', file);
    }
    return close_csv(file, path, error, error_size);
}

int csv_export_repeated_locations(
    const char *path,
    const CameraDataset *dataset,
    size_t *written,
    char *error,
    size_t error_size
) {
    FILE *file = open_csv(path, error, error_size);
    size_t index;

    *written = 0;
    if (file == NULL) {
        return 0;
    }
    fputs("id,location,year,zone,latitude,longitude,address\n", file);
    for (index = 0; index < dataset->count; index++) {
        if (query_location_is_shared(dataset, index)) {
            write_camera(file, &dataset->items[index]);
            fputc('\n', file);
            (*written)++;
        }
    }
    return close_csv(file, path, error, error_size);
}
