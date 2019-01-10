#include <math.h>
#include "functions.h"

#define TXPOWER -59

double norm (Point p) // get the norm of a vector
{
    double x = p.getX(), y=p.getY();
    return pow(pow(x,2)+pow(y,2) , 0.5);
}

Point trilateration(double r1, double r2, double r3) {
    Point resultPose, point1 = Point(esp1x, esp1y), point2 = Point(esp2x, esp2y), point3 = Point(esp3x, esp3y);
    //unit vector in a direction from point1 to point 2
    double p2p1Distance = pow(pow(point2.getX()-point1.getX(),2) + pow(point2.getY() -  point1.getY(),2),0.5);
    Point ex = {(point2.getX()-point1.getX())/p2p1Distance, (point2.getY()-point1.getY())/p2p1Distance};
    Point aux = {point3.getX()-point1.getX(),point3.getY()-point1.getY()};
    //signed magnitude of the x component
    double i = ex.getX() * aux.getX() + ex.getY() * aux.getY();
    //the unit vector in the y direction.
    Point aux2 = { point3.getX()-point1.getX()-i*ex.getX(), point3.getY()-point1.getY()-i*ex.getY()};
    Point ey = { aux2.getX() / norm(aux2), aux2.getY() / norm(aux2) };
    //the signed magnitude of the y component
    double j = ey.getX() * aux.getX() + ey.getY() * aux.getY();
    //coordinates
    double x = (pow(r1,2) - pow(r2,2) + pow(p2p1Distance,2))/ (2 * p2p1Distance);
    double y = (pow(r1,2) - pow(r3,2) + pow(i,2) + pow(j,2))/(2*j) - i*x/j;
    //result coordinates
    double finalX = point1.getX()+ x*ex.getX() + y*ey.getX();
    double finalY = point1.getY()+ x*ex.getY() + y*ey.getY();
    resultPose.setX(finalX);
    resultPose.setY(finalY);
    return resultPose;
}

double dbToMeters(int signal){
    double d;
    double n = 1.7;

    d = pow(10, (TXPOWER-signal)/(10*n));
    return d;
}

double calculateDistance(int rssi) {

  if (rssi == 0) {
    return -1.0;
  }

  double ratio = rssi*1.0/TXPOWER;
  if (ratio < 1.0) {
    return pow(ratio,10);
  }
  else {
    double distance =  (0.89976)*pow(ratio,7.7095) + 0.111;
    return distance;
  }
}

