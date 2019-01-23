#include "esp.h"

Esp::Esp()
{

}

Esp::Esp(QString name, QString mac, QPointF p)
{
    this->name = name;
    this->mac = mac;
    this->point = p;

}

QString Esp::getMac(){
    return this->mac;
}

QTcpSocket* Esp::getSocket(){
    return this->socket;
}

void Esp::setSocket(QTcpSocket *socket){
    this->socket = socket;
}

QPointF Esp::getPoint(){
    return this->point;
}
