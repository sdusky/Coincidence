#ifndef LOCALSOCKET_H
#define LOCALSOCKET_H

#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <common.h>
#include <QHostAddress>
class LocalSocket : public QTcpSocket
{
    Q_OBJECT
public:
    explicit LocalSocket(QObject *parent = 0);

    void connect_to_server(int port);                   //连接到服务器，数据库驱动层启动
private:

signals:   
    void connect_to_server_error(QString);              //系统错误：连接到服务端错误

    void post_message_to_jsonparser(QByteArray);        //信息传递至json类进行解封

public slots:
    void get_message_from_server();                     //服务器获取信息
    void get_message_from_jsonparser(QJsonObject);      //获取json封装的数据
    void catch_server_close_signal();                   //服务器关闭
};

#endif // LOCALSOCKET_H
