#ifndef INFORMATIONERROR_H
#define INFORMATIONERROR_H

#include <QObject>
#include <common.h>
#include <QDebug>
#include <QDateTime>
class InformationError : public QObject
{
    Q_OBJECT
public:
    explicit InformationError(QObject *parent = 0);

signals:
    //用户错误信号
    void tell_account_not_exist(QByteArray);                   //用户不存在  ，登录失败信号
    void tell_account_exist();                                 //用户已经存在，注册失败信号
    void tell_password_wrong(QByteArray);                      //用户密码错误，密码错误信号

    //诉求错误信号
    void tell_appeal_exist(QByteArray);                        //诉求已存在，插入错误信号
    void tell_appeal_insert(QByteArray);                       //诉求插入异常，告知用户

public slots:
    //用户信息处理
    void user_info_insert_error(QByteArray);                   //用户注册错误处理函数
    void user_account_check_error(QByteArray);                 //用户账号错误处理函数
    void user_password_check_error(QByteArray);                //用户密码错误处理函数

    //诉求信息处理
    void appeal_info_insert_error(appeal_info);                //诉求信息插入异常
    void appeal_info_exist(appeal_info);                       //该时刻诉求已经存在

};

#endif // INFORMATIONERROR_H
