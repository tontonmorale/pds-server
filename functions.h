#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include "point.h"

#define esp1x 0.0
#define esp1y 0.0
#define esp2x 2.0
#define esp2y 0.0
#define esp3x 2.0
#define esp3y 2.0

double norm(Point p);
Point trilateration(double r1, double r2, double r3);
double dbToMeters(int signal);
double calculateDistance(int rssi);
#endif // FUNCTIONS_H
