#ifndef VIASEGURA_QUERY_H
#define VIASEGURA_QUERY_H

#include "dataset.h"

#include <stddef.h>

typedef struct {
    const Camera *camera;
    double distance_km;
} CameraDistance;

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
);

int query_location_is_shared(const CameraDataset *dataset, size_t index);

#endif
