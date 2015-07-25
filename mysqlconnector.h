#ifndef MYSQLCONNECTOR_H
#define MYSQLCONNECTOR_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QTime>
#include <common.h>

class MySQLConnector : public QObject
{
    Q_OBJECT
public:
    explicit MySQLConnector(QObject *parent = 0);

private:
    bool connect_to_mysql();                                //连接数据库

    //user表操作
    bool user_register_insert(user_info);                   //插入
    bool user_login_check(user_info);                       //检查
    bool user_information_search(user_info );               //检索信息
    bool get_sexual_by_nickname(QByteArray, user_info *);   //查询性别

    //appeal表操作
    bool appeal_information_check(appeal_info);             //检查
    bool appeal_information_insert(appeal_info);            //插入
    bool appeal_information_update(partner,QString);        //更新两个用户的信息
    bool appeal_information_search(appeal_info);            //查找
    void appeal_information_search(partner *);              //通过appealID查找信息

    //successlist表操作
    bool success_information_insert(partner);               //插入成功列表
    void success_information_update(partner,QString);       //更新成功列表状态
    int  success_information_check(partner);                //更新
    void success_information_delete(partner);               //删除
    void success_information_search(partner *);             //配对信息查询

    int get_appeal_information_appealid(appeal_info);       //获取IP
    void get_phone_number(partner *);                       //获取电话
signals:
    void connect_to_db_error(QString);                      //系统错误：数据库连接错误

    //用户信息信号
        //错误信息
    void user_info_insert_error(QByteArray);                //信息错误：用户名存在，注册失败
    void user_account_check_error(QByteArray);              //信息错误：用户名不存在，登录失败
    void user_password_check_error(QByteArray);             //信息错误：密码错误，登录失败
        //下行信号
    void user_info_insert_success(QByteArray);              //注册成功，返回用户账号
    void user_info_login_success(QByteArray);               //登录成功，返回用户账号及当前匹配上的人

    //诉求信息信号
        //错误信息
    void appeal_info_insert_error(appeal_info);             //信息错误：诉求信息插入失败
    void appeal_info_exist(appeal_info);                    //信息错误：该时间段内诉求存在
    void appeal_info_update(QByteArray);                    //信息错误：诉求状态错误
        //下行信号
    void appeal_insert_success(appeal_info);                //信息反馈：诉求信息发布成功

    //诉求反馈信号
    void chat_start_signal(partner);                        //信息反馈：两者都碰一下，进入聊天
    void appeal_info_no_partner(QByteArray);                //信息反馈：没有合适的伙伴
    void appeal_info_post_partner(partner);                 //信息反馈：伙伴的昵称
    void send_personal_phone_to_jsonparser(user_info);      //信息反馈：返回信息
    void touch_no_happen_to_jsonparser(partner);            //下一个
    void touch_happen_to_jsonparser(partner);               //碰一下
    void appeal_happen(partner);                            //碰巧
    void appeal_no_happen(partner);                         //不巧

public slots:
    //用户信息处理函数
    void get_user_register_from_jsonparser(user_info);      //用户注册信息处理函数
    void get_user_login_from_jsonparser(user_info);         //用户登录信息处理函数

    //诉求信息处理函数
    void get_appeal_issue_from_jsonparser(appeal_info);     //诉求发布信息处理函数

    void get_appeal_next_from_jsonparser(appeal_info);      //获取下一个处理函数
    void get_touch_to_happen_from_jsonparser(partner);      //碰一下信号处理函数
    void get_appeal_happen_from_jsonparser(partner);        //碰巧信号处理函数
    void get_appeal_no_happen_from_jsonparser(partner);     //不巧信号处理函数
    void get_user_information_from_jsonparser(user_info);   //获取用户信息处理函数

};

#endif // MYSQLCONNECTOR_H
