#include "esp.h"

Esp::Esp()
{

}

Esp::Esp(QString id, QString mac, QPointF p)
{
    this->id = id;
    this->mac = mac;
    this->point = p;

}

QString Esp::getMac(){
    return this->mac;
}

QString Esp::getId(){
    return this->id;
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
