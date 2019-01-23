#ifndef ESP_H
#define ESP_H

#include <QPointF>
#include <QTcpSocket>


class Esp
{
public:
    Esp();
    Esp(QString name, QString mac, QPointF p);
    QString getMac();
    void setSocket(QTcpSocket *socket);
    QTcpSocket *getSocket();
    QPointF getPoint();


private:
    QString name;
    QString mac;
    QPointF point;
    QTcpSocket *socket;
};

#endif // ESP_H
