#include <math.h>
#include "functions.h"
#include <fstream>
#include <iostream>
#include <string>
#include <stdio.h>
#include <string.h>

using namespace std;

#define TXPOWER -42
#define CONF_FILE_PATH "C:/Users/tonio/Desktop/TcpServer/esp.txt"

double norm (Point p) // get the norm of a vector
{
    double x = p.getX(), y=p.getY();
    return pow(pow(x,2)+pow(y,2) , 0.5);
}

Point trilateration(double r1, double r2, double r3, QPointF esp1, QPointF esp2, QPointF esp3) {
    Point resultPose, point1 = Point(esp1.x(), esp1.y()), point2 = Point(esp2.x(), esp2.y()), point3 = Point(esp3.x(), esp3.y());
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
    double n = 1;

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

QList<Esp>* confFromFile(){
    //lettura posizione esp da file
    ifstream inFile;
    int i;
    string line, name, mac, x_str, y_str;
    int n_esp=0;
    double x_double, y_double;
    QList<Esp>* espList=new QList<Esp>();

    for(i=0; i<3; i++){
        inFile.open(CONF_FILE_PATH);
        if (!inFile) {
            cout << "Unable to open file";
        }
        else {
            break;
        }
    }
    if(i>=3){
        return nullptr;
    }

    inFile >> n_esp;
    while (inFile >> name >> mac >> x_str >> y_str) {
        x_double = stod(x_str.c_str());
        y_double = stod(y_str.c_str());

        QPointF p = QPointF(x_double, y_double);
        Esp esp = Esp(QString::fromStdString(name), QString::fromStdString(mac), p);
        espList->append(esp);
    }

    inFile.close();
    return espList;
}

