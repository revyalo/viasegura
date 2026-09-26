#ifndef VIASEGURA_CAMERA_H
#define VIASEGURA_CAMERA_H

#define CAMERA_ID_SIZE 32
#define CAMERA_YEAR_SIZE 16
#define CAMERA_LOCATION_SIZE 160
#define CAMERA_ZONE_SIZE 96
#define CAMERA_ADDRESS_SIZE 256

typedef struct {
    char id[CAMERA_ID_SIZE];
    char year[CAMERA_YEAR_SIZE];
    char location[CAMERA_LOCATION_SIZE];
    char zone[CAMERA_ZONE_SIZE];
    char address[CAMERA_ADDRESS_SIZE];
    double latitude;
    double longitude;
} Camera;

#endif
