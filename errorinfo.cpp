#include "errorinfo.h"

ErrorInfo::ErrorInfo(QObject *parent) :
    QObject(parent)
{
    qDebug("ErrorInfo object initialized!");
}

void ErrorInfo::get_error_info(QString err)
{
    qDebug()<<QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    qDebug()<<sender()->metaObject()->className()<<":";
    qDebug()<<err;
}

