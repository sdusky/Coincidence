#ifndef SYSTEMERROR_H
#define SYSTEMERROR_H

#include <QObject>
#include <QDateTime>
#include <QDebug>
#include <localsocket.h>

class SystemError : public QObject
{
    Q_OBJECT
public:
    explicit SystemError(QObject *parent = 0);

signals:

public slots:
    void db_connection_error(QString);              //数据库连接错误处理函数
    void server_connection_error(QString);          //服务器连接错误处理函数
    void json_parse_error(QString);                 //json格式解析错误
};

#endif // SYSTEMERROR_H
