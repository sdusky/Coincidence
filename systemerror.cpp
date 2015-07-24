#include "systemerror.h"

SystemError::SystemError(QObject *parent) :
    QObject(parent)
{
    qDebug("System error handler initialize.");
}

void SystemError::db_connection_error(QString err)
{
    //打印错误信息
    qDebug("--------------------------------------------------------------------");
    qDebug()<< "Time:" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    qDebug()<< "system_error: " << err;
}

void SystemError::server_connection_error(QString err)
{
    //打印错误信息
    qDebug("--------------------------------------------------------------------");
    qDebug()<< "Time:" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    qDebug()<< "system_error: " << err;
    qDebug()<< "Try to connect to server!";

    static_cast <LocalSocket *>(sender())->connect_to_server(server_port);
}

void SystemError::json_parse_error(QString err)
{
    //打印错误信息
    qDebug("--------------------------------------------------------------------");
    qDebug()<< "Time:" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    qDebug()<< "system_error: " << err;
}
