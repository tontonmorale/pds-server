#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QDebug>
#include <QList>
#include <QtSql>
#include <QIODevice>
#include <QNetworkInterface>
#include <QHostAddress>
#include <QMap>
#include <QSet>
#include <QSharedPointer>
#include "packet.h"
#include "person.h"
#include "functions.h"
#include "window.h"
#include "esp.h"

class MyTcpServer : public QObject
{
    Q_OBJECT
public:
    MyTcpServer(QObject *parent = nullptr);
    ~MyTcpServer();
    bool dbConnect();
    bool serverListen(QTcpServer *server, quint16 port);
    bool serverInit();
    QHostAddress getLocalIp();
    void startToClients();

signals:

public slots:
    void onClientConnection();
    void onClientDisconnection(QAbstractSocket::SocketState sockError);
    void readFromClient();


private:
    Window window;
    QTcpServer *server;
    QList<QTcpSocket *> socketList;
    int connectedClients;
    int receivedPacketsFiles;
    int currMinute;
    int currTimeSlot;
    QSqlDatabase db;
    int dbInsertPacket(QString hash, QString mac, QString microsec, QString signal, QString ssid);
    QMap<QString, QSharedPointer<Packet>> packetsMap;
    QMap<QString, int> areaPacketsMap;
    QMap<QString, Person> peopleMap;
    QList<Esp> *espList;
    QList<QPointF> peopleCounter;
    void updatePacketsSet(Person &p, QString shortKey);
};

#endif // MYTCPSERVER_H
