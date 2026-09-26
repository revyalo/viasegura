#include "dataset.h"
#include "query.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    CameraDataset dataset = {0};
    CameraDistance *results = NULL;
    char error[256];
    size_t count = 0;
    size_t index;

    assert(argc == 2);
    assert(dataset_load(argv[1], &dataset, error, sizeof(error)));
    assert(dataset.count > 0);
    assert(query_nearby(&dataset, 40.4168, -3.7038, 5.0, "LAVAPIES",
        &results, &count, error, sizeof(error)));
    assert(count > 0);
    for (index = 0; index < count; index++) {
        assert(strcmp(results[index].camera->zone, "LAVAPIES") == 0);
        assert(results[index].distance_km <= 5.0);
        if (index > 0) {
            assert(results[index - 1].distance_km <= results[index].distance_km);
        }
    }
    free(results);

    results = NULL;
    count = 0;
    assert(!query_nearby(&dataset, 91.0, 0.0, 1.0, NULL,
        &results, &count, error, sizeof(error)));
    dataset_free(&dataset);
    puts("dataset/query tests: OK");
    return 0;
}
