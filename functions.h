#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include "point.h"
#include "esp.h"
#include <QList>
#include <QPointF>
#include "window.h"

//#define esp1x 0.0
//#define esp1y 0.0
//#define esp2x 2.0
//#define esp2y 0.0
//#define esp3x 2.0
//#define esp3y 2.0

#define esp1x 0.0
#define esp1y 0.0
#define esp2x 2.0
#define esp2y 0.0
#define esp3x 0.0
#define esp3y 2.0

double norm(Point p);
Point trilateration(double r1, double r2, double r3, QPointF esp1, QPointF esp2, QPointF esp3);
double dbToMeters(int signal);
double calculateDistance(int rssi);
QList<Esp>* confFromFile();
QPointF setCoordinates(QList<Esp> *p);
#endif // FUNCTIONS_H
