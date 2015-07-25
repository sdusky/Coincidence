#include "localsocket.h"

LocalSocket::LocalSocket(QObject *parent) :
    QTcpSocket(parent)
{
    qDebug("LocalSocket object initialized!");

    //connect signal and slots
    connect(this,SIGNAL(readyRead()),this,SLOT(get_message_from_server()));
    connect(this,SIGNAL(disconnected()),this,SLOT(catch_server_close_signal()));

}

void LocalSocket::get_message_from_server()
{
    QByteArray msg;

    msg = this->readAll();
    emit post_message_to_jsonparser(msg);
}

void LocalSocket::get_message_from_jsonparser(QJsonObject json_obj)
{
    QJsonDocument json_doc;

    json_doc.setObject(json_obj);
    QByteArray msg_to_server = json_doc.toJson(QJsonDocument::Compact);

    this->write(msg_to_server);
}

void LocalSocket::catch_server_close_signal()
{
    qDebug()<<"Server Closed";
    connect_to_server(server_port);
}

void LocalSocket::connect_to_server(int port)
{
    //connectToHost("127.0.0.1",port);
    connectToHost("172.28.141.2",port);

    if(!waitForConnected(3000))
    {        
        emit connect_to_server_error("Connect to localhost error! Please check your server!");
    }
    else
    {
        qDebug("Connect to local server, start coincidence!");
    }
}
