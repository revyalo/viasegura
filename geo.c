#include "geo.h"
#include <math.h>

#define PI 3.14159265358979323846
#define RADIO_TIERRA_KM 6371.0

static double gradosARadianes(double grados) {
    return grados * (PI / 180.0);
}

double distanciaGeografica(double latA, double longA, double latB, double longB) {
    double latARad = gradosARadianes(latA);
    double latBRad = gradosARadianes(latB);
    double incLat = gradosARadianes(latB - latA);
    double incLong = gradosARadianes(longB - longA);
    double a = pow(sin(incLat / 2.0), 2.0)
        + cos(latARad) * cos(latBRad) * pow(sin(incLong / 2.0), 2.0);
    double c = 2.0 * atan2(sqrt(a), sqrt(1.0 - a));

    return RADIO_TIERRA_KM * c;
}
