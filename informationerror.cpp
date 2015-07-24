#include "informationerror.h"

InformationError::InformationError(QObject *parent) :
    QObject(parent)
{
    qDebug()<< "Time:" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    qDebug("Information error handler initialized.");
}

void InformationError::user_account_check_error(QByteArray user_name)
{
    //打印错误信息
    qDebug("--------------------------------------------------------------------");
    qDebug()<< "Time:" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    qDebug()<< "information_error: " << user_name << " does not exist!";

    //发出用户不存在错误
    emit tell_account_not_exist(user_name);
}

void InformationError::user_info_insert_error(QByteArray user_account)
{
    //打印错误信息
    qDebug("--------------------------------------------------------------------");
    qDebug()<< "Time:" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    qDebug()<< "information_error: " << user_account << " exist!";

    //发出用户已存在错误
    emit tell_account_exist();
}

void InformationError::user_password_check_error(QByteArray user_name)
{
    //打印错误信息
    qDebug("--------------------------------------------------------------------");
    qDebug()<< "Time:" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    qDebug()<< "information_error: " << user_name << " password error!";

    //发出用户密码错错误
    emit tell_password_wrong("password_error");
}

void InformationError::appeal_info_exist(appeal_info info)
{
    //打印错误信息
    qDebug("--------------------------------------------------------------------");
    qDebug()<< "Time:" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    qDebug("information_error: ");
    qDebug()<< "姓名：" << info.user_account;
    qDebug()<< "诉求：" << info.appeal_thing;
    qDebug("Insert error!Appeal already exists!");

    //发错诉求已存在错误
    emit tell_appeal_exist(info.user_account);
}

void InformationError::appeal_info_insert_error(appeal_info info)
{
    //打印错误信息
    qDebug("--------------------------------------------------------------------");
    qDebug()<< "Time:" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    qDebug("information_error: ");
    qDebug("姓名：" + info.user_account);
    qDebug("诉求：" + info.appeal_thing);
    qDebug("Insert error!Please check database and server!");
    //TODO
    //发出数据库插入异常错误
    emit tell_appeal_insert(info.user_account);
}
