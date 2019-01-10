#include <QApplication>
#include <stdio.h>
#include <string.h>
#include "mytcpserver.h"
#include <QtSql>
#include <QSqlDatabase>
#include <QDebug>
#include <QNetworkInterface>
#include <QHostAddress>
#include <QHostInfo>
#include "point.h"
#include "functions.h"
#include "window.h"

using namespace std;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MyTcpServer myServer;

    return app.exec();
}
