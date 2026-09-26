#include "query.h"

#include "geo.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int compare_distance(const void *left_value, const void *right_value) {
    const CameraDistance *left = left_value;
    const CameraDistance *right = right_value;

    if (left->distance_km < right->distance_km) {
        return -1;
    }
    if (left->distance_km > right->distance_km) {
        return 1;
    }
    return strcmp(left->camera->id, right->camera->id);
}

int query_nearby(
    const CameraDataset *dataset,
    double latitude,
    double longitude,
    double radius_km,
    const char *zone,
    CameraDistance **results,
    size_t *count,
    char *error,
    size_t error_size
) {
    CameraDistance *matches;
    size_t index;

    *results = NULL;
    *count = 0;
    if (!geo_coordinates_valid(latitude, longitude) || radius_km < 0.0) {
        snprintf(error, error_size, "Parametros geograficos fuera de rango");
        return 0;
    }
    matches = malloc(dataset->count * sizeof(*matches));
    if (matches == NULL && dataset->count > 0) {
        snprintf(error, error_size, "Memoria insuficiente para la consulta");
        return 0;
    }
    for (index = 0; index < dataset->count; index++) {
        double distance;
        if (zone != NULL && strcmp(zone, dataset->items[index].zone) != 0) {
            continue;
        }
        distance = distanciaGeografica(latitude, longitude,
            dataset->items[index].latitude, dataset->items[index].longitude);
        if (distance <= radius_km) {
            matches[*count].camera = &dataset->items[index];
            matches[*count].distance_km = distance;
            (*count)++;
        }
    }
    qsort(matches, *count, sizeof(*matches), compare_distance);
    *results = matches;
    return 1;
}

int query_location_is_shared(const CameraDataset *dataset, size_t index) {
    size_t other;

    for (other = 0; other < dataset->count; other++) {
        if (other != index && strcmp(dataset->items[index].location,
            dataset->items[other].location) == 0) {
            return 1;
        }
    }
    return 0;
}
