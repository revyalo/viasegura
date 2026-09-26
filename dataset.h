#ifndef VIASEGURA_DATASET_H
#define VIASEGURA_DATASET_H

#include "camera.h"

#include <stddef.h>

typedef struct {
    Camera *items;
    size_t count;
    size_t capacity;
} CameraDataset;

int dataset_load(const char *path, CameraDataset *dataset, char *error, size_t error_size);
void dataset_free(CameraDataset *dataset);

#endif
