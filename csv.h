#ifndef VIASEGURA_CSV_H
#define VIASEGURA_CSV_H

#include "query.h"

#include <stddef.h>

int csv_export_nearby(
    const char *path,
    const CameraDistance *results,
    size_t count,
    char *error,
    size_t error_size
);

int csv_export_repeated_locations(
    const char *path,
    const CameraDataset *dataset,
    size_t *written,
    char *error,
    size_t error_size
);

#endif
