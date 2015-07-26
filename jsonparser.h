#ifndef JSONPARSER_H
#define JSONPARSER_H

#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonArray>
#include <QDebug>
#include <QStringList>
#include <QMap>
#include <common.h>
#include <QDateTime>

class JsonParser : public QObject
{
    Q_OBJECT
public:
    explicit JsonParser(QObject *parent = 0);
    QString json_to_bytearray(QJsonDocument);

private:
    QMap<QString,int> map;

    //用户信息处理
    void user_register(QJsonObject);                                    //注册
    void user_login(QJsonObject);                                       //登录
    void user_informtion(QJsonObject);                                  //获取个人信息

    //诉求信息处理
    void appeal_issue(QJsonObject);                                     //发布
    void appeal_next(QJsonObject);                                      //推送需求，下一个

    //碰巧or不巧
    void appeal_happen(QJsonObject);                                    //碰巧
    void appeal_no_happen(QJsonObject);                                 //不巧
    void touch_to_happen(QJsonObject);                                  //碰一下

    //json信息解析
    void get_user_info(user_info *info,QJsonObject json_obj);           //用户信息解析
    void get_appeal_info(appeal_info *info,QJsonObject json_obj);       //诉求信息解析
    void get_partner_info(partner *info,QJsonObject json_obj);          //同伴信息解析

signals:    
    void json_parse_error(QString);                                     //系统错误：json数据解析错误信号
    //上行信号
    void user_register_to_mysqlconnector(user_info);                    //向数据库传递信号：用户注册信息
    void user_login_to_mysqlconnector(user_info);                       //向数据库传递信号：用户登录信息

    void appeal_issue_to_mysqlconnector(appeal_info);                   //向数据库传递信号：诉求发布信息

    void appeal_next_to_mysqlconnector(appeal_info);                    //向数据库传递信号：请求下一个
    void touch_to_happen_to_mysqlconnector(partner);                    //向数据库传递信号：碰一下

    void appeal_happen_to_mysqlconnector(partner);                      //向数据库传递信号：碰巧
    void appeal_no_happen_to_mysqlconnector(partner);                   //向数据库传递信号：不巧

    void user_information_to_mysqlconnector(user_info);                 //向数据库传递信号：请求名片

    //下行信号
    void post_message_to_localsocket(QJsonObject);

public slots:
    void get_message_from_localsocket(QByteArray);                      //服务器上传信息处理函数

    void create_packet_register_success(QByteArray);                    //数据包生成：注册成功
    void create_packet_register_fail();                                 //数据包生成：注册失败
    void create_packet_login_success(QByteArray,QJsonArray);            //数据包生成：登录成功
    void create_packet_login_fail(QByteArray);                          //数据包生成：登录失败
    void create_packet_appeal_success(appeal_info);                     //数据包生成：发布成功
    void create_packet_appeal_fail(QByteArray);                         //数据包生成：发布失败
    void create_packet_appeal_post(partner);                            //数据包生成：诉求推送
    void create_packet_touch(partner);                                  //数据包生成：碰一下反馈
    void create_packet_touch_fail(partner);                             //数据包生成：下一个反馈
    void create_packet_no_happen(partner);                              //数据包生成：不巧反馈
    void create_packet_happen(partner);                                 //数据包生成：碰巧反馈
    void create_packet_information(partner);                            //数据包生成：互发名片
    void create_packet_personal_information(user_info);                 //数据包生成：发送个人信息
    void create_packet_start_chat(partner);                             //数据包生成：允许聊天
};

#endif // JSONPARSER_H
