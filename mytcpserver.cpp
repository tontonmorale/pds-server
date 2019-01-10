#include "mytcpserver.h"
#include "person.h"
#include "packet.h"
#include <QPointF>
#include "stdio.h"
#include <iostream>

#define MAX_CLIENTS 3
#define MAX_MINUTES 5
#define PORT 9999
#define TIME_SLOT 5
using namespace std;

MyTcpServer::MyTcpServer(QObject *parent) :
    QObject(parent)
{
    this->server = new QTcpServer(this);
    this->connectedClients = 0;
    this->receivedPacketsFiles = 0;
    this->currMinute = 0;
    this->currTimeSlot = 0;

    QPointF point = QPointF(0, 0);
    this->peopleCounter.append(point);
    //this->peopleMap = new QMap<QString, QSet<Packet>>();

    serverInit();
    this->window.show();
}

bool MyTcpServer::serverInit(){
    int i=0;

    // signal for new client connection
    connect(server, SIGNAL(newConnection()), this, SLOT(onClientConnection()));

    //start listenint to incoming connections
    for(i=0; i<3; i++)
        if(serverListen(server, PORT))
            break;
    if(i==3)
        return false;

    return true;
}

bool MyTcpServer::serverListen(QTcpServer *server, quint16 port){
    bool flag;
    QHostAddress ip = getLocalIp();

    if(ip == QHostAddress::Null){
        qDebug().noquote() << "Cannot get local ip address\n";
        flag = false;
    }

    if(!server->listen(ip, port)){
        qDebug().noquote() << "Error on listen, server could not start\n";
        flag = false;
    }

    else {
        qDebug().noquote() << "Server started\n";
        qDebug() << "Waiting for connections ...";
        flag = true;
    }

    return flag;
}

void MyTcpServer::onClientConnection() {

    QTcpSocket *socket = server->nextPendingConnection();

    QString clientId, welcomeMsg;
    const char* msg;

//    qDebug().noquote() << socket->state();
//    char stringa[10];



    connect(socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(onClientDisconnection(QAbstractSocket::SocketState)));
//    connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &MyTcpServer::onClientDisconnection);
//    connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
//        [=](QAbstractSocket::SocketError socketError){
//        qDebug().noquote() << "segnale di errore ricevuto";
//    });
    connect(socket, SIGNAL(readyRead()), this, SLOT(readFromClient()));

    this->socketList.append(socket);
    this->connectedClients++;

//    while(1){
//        cin >> stringa ;
//        qDebug().noquote() << (*(this->socketList.begin()))->state();
//    }

    qDebug().noquote() << "New connection from client " + QString::number(this->connectedClients) + "\n";

    //new client, send hello and esp id
    clientId = QString("%1").arg(connectedClients, 2, 10, QChar('0'));
    welcomeMsg = "CIAO " + clientId + "\r\n";
    qDebug().noquote() << "welcomeMsg: " + welcomeMsg;
    msg = welcomeMsg.toStdString().c_str();
    qDebug() << msg;
    socket->write(msg);

    //all clients connected, send start
    if(this->connectedClients >= MAX_CLIENTS){
        this->currMinute=1;
        startToClients();
    }
}

void MyTcpServer::startToClients(){
    for (QTcpSocket *s : this->socketList) {
        qDebug().noquote() << "invio start";
        s->write("START\r\n");
    }
}

void MyTcpServer::onClientDisconnection(QAbstractSocket::SocketState sockError) {
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(QObject::sender());

    int i = this->socketList.indexOf(socket);
    if (i!=-1)
        this->socketList.removeAt(i);

    qDebug().noquote() << "Client number " + QString::number(this->connectedClients) + " disconnected";

    this->connectedClients--;

    socket->deleteLater();
}

bool MyTcpServer::dbConnect() {
    this->db = QSqlDatabase::addDatabase("QMYSQL");
    this->db.setHostName("localhost");
    this->db.setDatabaseName("databasepds");
    this->db.setUserName("root");
    this->db.setPassword("");

    if (db.open()){
        qDebug().noquote() << "Connection to db extablished\n";
        return true;
    }
    else{
        qDebug().noquote() << "Impossible to connect to db\n";
        return false;
    }
}

void MyTcpServer::readFromClient(){
    QTcpSocket* conn = qobject_cast<QTcpSocket*>(sender());
    QString line, firstWord, hash, timestamp, mac, signal, microsec_str, esp, ssid;
    Packet* pkt;// = new Packet();
    QStringList sl, tsSplit, macList;
    Packet p;

    while ( conn->canReadLine() ) {
        line = QString(conn->readLine());
        line.remove('\n');
        qDebug().noquote() << line + "\n";

//        cout << tr("ciao");
        firstWord = line.split(" ").at(0);

        //received new packet
        if(firstWord.compare("PKT")==0){

            line.remove(',');
            sl=line.split(" ");
            if(sl.length()!=6){
                //wrong parameters number
                continue;
            }

            hash = sl.at(1);
            mac = sl.at(2);
            signal = sl.at(3);
            timestamp = sl.at(4);
            esp = sl.at(5);

//            tsSplit = timestamp.split(':');
//            microsec_long = (tsSplit.at(0)).toLongLong()*1000000 + (tsSplit.at(1)).toLongLong();
//            microsec_str = QString::number(microsec_long);

            QString key = hash + "-" + mac + "-" + esp;
            QString shortKey = hash + "-" + mac;

            //insert new packet if in first minute or person in the area in previous minutes
//            if(this->currMinute==1 || (this->currMinute!=1 && this->peopleMap.find(mac)!=this->peopleMap.end())){
            if(1){
                if(this->packetsMap.find(key) == this->packetsMap.end()){
                    //insert new packet
                    pkt = new Packet(hash, mac, timestamp, signal, esp, "ssid");
                    this->packetsMap[key] = QSharedPointer<Packet>(pkt);

                    //update area packet count
                    if(this->areaPacketsMap.find(shortKey) != this->areaPacketsMap.end()){
                        this->areaPacketsMap[shortKey] ++;
                    }
                    else{
                        this->areaPacketsMap[shortKey] = 1;
                    }
                }
            }
        }

        //received all packets
        if(firstWord.compare("END")==0){

            this->receivedPacketsFiles++;
            if(receivedPacketsFiles>=MAX_CLIENTS){
                this->receivedPacketsFiles = 0;
                //insert in peopleMap if packet received by all clients
                QMap<QString, int>::iterator i;
                for(i=this->areaPacketsMap.begin(); i!=this->areaPacketsMap.end(); i++){
                    if(i.value() >= MAX_CLIENTS){

                        QString shortKey = i.key();
                        QString mac = shortKey.split('-').at(1);

                        //if mac is not in peopleMap
                        if(this->peopleMap.find(mac) == this->peopleMap.end()){

                            Person p = Person(mac);
                            updatePacketsSet(p, shortKey);
                            //insert new person in peopleMap
                            peopleMap[mac] = p;
                        }
                        else{
                            //check if mac already considered in current minute
                            int count = this->peopleMap[mac].getMinCount();
                            if(count < this->currMinute){
                                Person p = this->peopleMap[mac];
                                this->peopleMap[mac].setMinCount(count+1);
                                updatePacketsSet(this->peopleMap[mac], shortKey);
                            }
                        }
                    }
                }



                //delete people with minutesCount != current minute
//                QMap<QString, Person>::iterator pm;
//                for(pm=this->peopleMap.begin(); pm!=this->peopleMap.end(); ){
//                    if(pm.value().getMinCount() != this->currMinute)
//                       pm = this->peopleMap.erase(pm);
//                    else
//                        pm++;
//                }

                qDebug().noquote() << "Fine minuto: " + QString::number(this->currMinute) + "\n";
                qDebug().noquote() << "-------------\n\n";


                //if last minute
                if(this->currMinute >= MAX_MINUTES){                    

                    //calculate positions of people in area
                    QList<QPointF> points;
                    QMap<QString, Person>::iterator pm;
                    for(pm=this->peopleMap.begin(); pm!=this->peopleMap.end(); pm++){
                        qDebug().noquote() << "Mac: " + pm.key();
                        qDebug().noquote() << " con i seguenti pacchetti:\n";

                        QSet<QSharedPointer<Packet>> ps = pm.value().getPacketsSet();

                        double d1, d2, d3;

                        //corrispondenza tra un certo esp e la sua potenza
                        for (QSet<QSharedPointer<Packet>>::iterator i = ps.begin(); i != ps.end(); i++) {
                            if ((*i)->getEsp().compare("01")==0){
//                                d1 = dbToMeters((*i)->getSignal());
                                d1 = calculateDistance((*i)->getSignal());
//                                if((*i)->getMac().compare("30:74:96:94:e3:2d")==0)
                                    qDebug().noquote() << QString::number(d1) + " " + QString::number((*i)->getSignal()) + "\n";
                            }
                            else if ((*i)->getEsp().compare("02")==0){
//                                d2 = dbToMeters((*i)->getSignal());
                                d2 = calculateDistance((*i)->getSignal());
//                                if((*i)->getMac().compare("30:74:96:94:e3:2d")==0)
                                    qDebug().noquote() << QString::number(d2) + " " + QString::number((*i)->getSignal()) + "\n";
                            }
                            else if ((*i)->getEsp().compare("03")==0){
//                                d3 = dbToMeters((*i)->getSignal());
                                d3 = calculateDistance((*i)->getSignal());
//                                if((*i)->getMac().compare("30:74:96:94:e3:2d")==0)
                                    qDebug().noquote() << QString::number(d3) + " " + QString::number((*i)->getSignal()) + "\n";
                            }
                            qDebug().noquote() << (*i)->getHash() + "\n";

                        }
                        Point res = trilateration(d1, d2, d3);
                        QPointF point = QPointF(res.getX(), res.getY());
                        points.append(point);
                    }

                    //conta le persone nell'area e fai grafichino bello
                    this->currTimeSlot ++;
                    QPointF point = QPointF(this->currTimeSlot * TIME_SLOT, peopleMap.size());
                    this->peopleCounter.append(point);
                    this->window.setWidget("time", peopleCounter);
                    //disegna persone nella mappa
                    this->window.setWidget("map", points);

                    if(this->currTimeSlot>=12){
                        this->currTimeSlot = 0;
                        this->peopleCounter.clear();
                        QPointF point = QPointF(0, 0);
                        this->peopleCounter.append(point);
                    }

                    //delete previous packets
                    this->peopleMap.clear();                    

                    this->currMinute = 0;
                    //send start to clients
                    startToClients();
                    //...todo
                    //...todo
                    //...todo
                }
                this->currMinute++;
                this->packetsMap.clear();
                this->areaPacketsMap.clear();
                conn->flush();
            }
        }



        // parte commentata sotto da eseguire solo dopo aver letto tutte le righe da tutti i client, quindi non in questo while

//        QString querystring2 = "SELECT ESP, HASH, MAC FROM pacchetti GROUP BY HASH, MAC HAVING COUNT(*) = 4" + MAX_CLIENTS;
//        QString querystring3;
//        qDebug() << querystring2;
//       if (query.exec(querystring2)) {
//           qDebug() << "Query2 andata a buon fine";
//           querystring3 = "INSERT INTO mac(MAC) VALUES (";
//           while (query.next()) {
//                querystring3+= "'" + query.value("MAC").toString() + "'), ";
//           }
//        }
//       else
//           qDebug() << "Query2 fallita";
//       querystring3.remove(querystring3.length()-2, 2);
//       qDebug() << querystring3;
//       if (query.exec(querystring3))
//           qDebug() << "Query3 andata a buon fine";
//       else
//           qDebug() << "Query3 fallita";


    }
}

void MyTcpServer::updatePacketsSet(Person &p, QString shortKey){
    QMap<QString, QSharedPointer<Packet>>::iterator itLow, i;
    int n;
    itLow = this->packetsMap.lowerBound(shortKey);

    p.clearPacketsSet();

    for(i=itLow, n=0; n<MAX_CLIENTS; i++, n++)
        p.insertPacket(i.value());

    //check if set contains n=MAX_CLIENTS packets
//    if(packetsSet.size()!=MAX_CLIENTS)
//        continue;
}

int MyTcpServer::dbInsertPacket(QString hash, QString mac, QString microsec, QString signal, QString ssid){

    QSqlQuery query;
    QString queryString;

    queryString = "INSERT INTO pacchetti (ESP, HASH, MAC, TIMESTAMP, SEGNALE) "
                          "VALUES (1, '" + hash + "', '" + mac + "', " + microsec + ", " + signal + ")"; //???????? esp value

    qDebug().noquote() << "query: " + queryString;

    if (query.exec(queryString)) {
        qDebug() << "Query andata a buon fine";
        return 1;
    }
    else{
        qDebug() << "Query fallita";
        return 0;
    }
}

QHostAddress MyTcpServer::getLocalIp(){

    QNetworkInterface interface;
    QNetworkAddressEntry entry;
    bool found=false;

    foreach(interface, QNetworkInterface::allInterfaces()) {
        if (interface.flags().testFlag(QNetworkInterface::IsUp) && interface.type()==QNetworkInterface::Wifi){
            foreach (entry, interface.addressEntries()) {
                // if is ipv4 but not localhost
                if (entry.ip().toString().contains('.') && entry.ip() != QHostAddress(QHostAddress::LocalHost)){
                    qDebug().noquote() << "--- Network interface info ---";
                    qDebug().noquote() << "name : " + interface.name();
                    qDebug().noquote() << "ip : " + entry.ip().toString();
                    qDebug().noquote() << "mac : " + interface.hardwareAddress();
                    qDebug().noquote() << "------------------------------\n";
                    found = true;
                    break;
                }
            }
        }
        if(found)
            break;
    }
    if(!found)
        return QHostAddress::Null;
    return entry.ip();
}

MyTcpServer::~MyTcpServer(){
    delete this->server;
    this->server = nullptr;
}
