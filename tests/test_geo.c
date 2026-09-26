#include "geo.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>

static int near(double actual, double expected, double tolerance) {
    return fabs(actual - expected) <= tolerance;
}

int main(void) {
    assert(near(distanciaGeografica(40.4168, -3.7038, 40.4168, -3.7038), 0.0, 0.000001));
    assert(near(distanciaGeografica(0.0, 0.0, 0.0, 1.0), 111.195, 0.01));
    assert(near(distanciaGeografica(90.0, 0.0, -90.0, 0.0), 20015.087, 0.02));
    assert(geo_coordinates_valid(-90.0, -180.0));
    assert(geo_coordinates_valid(90.0, 180.0));
    assert(!geo_coordinates_valid(90.1, 0.0));
    assert(!geo_coordinates_valid(0.0, 180.1));
    assert(!geo_coordinates_valid(NAN, 0.0));
    puts("geo tests: OK");
    return 0;
}
