#ifndef PERSON_H
#define PERSON_H

#include <QString>
#include <QSharedPointer>
#include <QSet>
#include "packet.h"
#include "point.h"


class Person
{
public:
    Person();
    Person(QString mac);
    int getMinCount();
    void setMinCount(int minutesCount);
    void flushPacketsSet();
    void setPacketsSet(QSet<QSharedPointer<Packet>> packetsSet);
    QSet<QSharedPointer<Packet>> getPacketsSet();
    void insertPacket(QSharedPointer<Packet> p);
    void clearPacketsSet();

private:
    QString mac;
    QSet<QSharedPointer<Packet>> packetsSet;
    int minutesCount;
    Point p;
};

#endif // PERSON_H
