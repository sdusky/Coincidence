#include <QCoreApplication>
#include <informationerror.h>
#include <systemerror.h>
#include <localsocket.h>
#include <jsonparser.h>
#include <mysqlconnector.h>
#include <QObject>


/*
 * 数据上行顺序 接收-->解析-->数据库
 * MySQLConnector类
 *       /\
 *        |
 *        |
 * JsonParser类
 *       /\
 *        |
 *        |
 * LocalSocket类
 *
 */

/*
 * 数据下行顺序 取数据库-->封装-->发送
 * MySQLConnector类
 *        |
 *        |         InformationError类
 *        \/
 * JsonParser类
 *        |
 *        |
 *        \/
 * LocalSocket类
 *
 */

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    InformationError        information_error;
    SystemError             system_error;
    JsonParser              json_parser;
    MySQLConnector          mysql_connector;
    LocalSocket             local_socket;


    //local_socket--->system_error      连接服务器错误 系统错误信号与处理
    QObject::connect(&local_socket,SIGNAL(connect_to_server_error(QString)),
                     &system_error,SLOT(server_connection_error(QString)));

    //mysql_connector--->system_error   连接数据库错误 系统错误信号与处理
    QObject::connect(&mysql_connector,SIGNAL(connect_to_db_error(QString)),
                     &system_error,SLOT(db_connection_error(QString)));

    //json_parser--->system_error       解析错误      系统错误信号与处理
    QObject::connect(&json_parser,SIGNAL(json_parse_error(QString)),
                     &system_error,SLOT(json_parse_error(QString)));

    //local_socket--->json              上行数据 socket->json 接收来自服务器的信息，送json类解析
    QObject::connect(&local_socket,SIGNAL(post_message_to_jsonparser(QByteArray)),
                     &json_parser,SLOT(get_message_from_localsocket(QByteArray)));

    //json_parser--->mysql_connector    上行数据        json->db
    //                                  解析完用户注册信息，送数据库插入
    QObject::connect(&json_parser,SIGNAL(user_register_to_mysqlconnector(user_info)),
                     &mysql_connector,SLOT(get_user_register_from_jsonparser(user_info)));
    //                                  解析完用户登录信息，送数据库检查
    QObject::connect(&json_parser,SIGNAL(user_login_to_mysqlconnector(user_info)),
                     &mysql_connector,SLOT(get_user_login_from_jsonparser(user_info)));
    //                                  解析完诉求信息，送数据库检查后插入
    QObject::connect(&json_parser,SIGNAL(appeal_issue_to_mysqlconnector(appeal_info)),
                     &mysql_connector,SLOT(get_appeal_issue_from_jsonparser(appeal_info)));
    //                                  下一个推送请求
    QObject::connect(&json_parser,SIGNAL(appeal_next_to_mysqlconnector(appeal_info)),
                     &mysql_connector,SLOT(get_appeal_next_from_jsonparser(appeal_info)));
    //                                  用户碰一下
    QObject::connect(&json_parser,SIGNAL(touch_to_happen_to_mysqlconnector(partner)),
                     &mysql_connector,SLOT(get_touch_to_happen_from_jsonparser(partner)));
    //                                  用户碰巧
    QObject::connect(&json_parser,SIGNAL(appeal_happen_to_mysqlconnector(partner)),
                     &mysql_connector,SLOT(get_appeal_happen_from_jsonparser(partner)));
    //                                  用户信息请求
    QObject::connect(&json_parser,SIGNAL(user_information_to_mysqlconnector(user_info)),
                     &mysql_connector,SLOT(get_user_information_from_jsonparser(user_info)));

    //mysql_connector--->json_parser    下行数据        db->json 成功
    //                                  用户注册成功
    QObject::connect(&mysql_connector,SIGNAL(user_info_insert_success(QByteArray)),
                     &json_parser,SLOT(create_packet_register_success(QByteArray)));
    //                                  用户登录成功
    QObject::connect(&mysql_connector,SIGNAL(user_info_login_success(QByteArray)),
                     &json_parser,SLOT(create_packet_login_success(QByteArray)));
    //                                  诉求发布成功
    QObject::connect(&mysql_connector,SIGNAL(appeal_insert_success(appeal_info)),
                     &json_parser,SLOT(create_packet_appeal_success(appeal_info)));
    //                                  诉求推送
    QObject::connect(&mysql_connector,SIGNAL(appeal_info_post_partner(partner)),
                     &json_parser,SLOT(create_packet_appeal_post(partner)));
    //                                  碰一下推送
    QObject::connect(&mysql_connector,SIGNAL(touch_happen_to_jsonparser(partner)),
                     &json_parser,SLOT(create_packet_touch(partner)));
    //                                  下一个推送
    QObject::connect(&mysql_connector,SIGNAL(touch_no_happen_to_jsonparser(partner)),
                     &json_parser,SLOT(create_packet_no_happen(partner)));
    //                                  不巧推送
    QObject::connect(&mysql_connector,SIGNAL(appeal_no_happen(partner)),
                     &json_parser,SLOT(create_packet_no_happen(partner)));
    //                                  聊天推送
    QObject::connect(&mysql_connector,SIGNAL(chat_start_signal(partner)),
                     &json_parser,SLOT(create_packet_start_chat(partner)));
    //                                  碰巧推送，推送信息
    QObject::connect(&mysql_connector,SIGNAL(appeal_happen(partner)),
                     &json_parser,SLOT(create_packet_information(partner)));
    //                                  个人信息推送
    QObject::connect(&mysql_connector,SIGNAL(send_personal_phone_to_jsonparser(user_info)),
                     &json_parser,SLOT(create_packet_personal_information(user_info)));

    //mysql_connector--->information_error     信息错误信号与处理 下行数据 db->err
    //                                  用户注册信息插入错误
    QObject::connect(&mysql_connector,SIGNAL(user_info_insert_error(QByteArray)),
                     &information_error,SLOT(user_info_insert_error(QByteArray)));
    //                                  用户登录信息检查错误
    QObject::connect(&mysql_connector,SIGNAL(user_account_check_error(QByteArray)),
                     &information_error,SLOT(user_account_check_error(QByteArray)));
    //                                  用户登录信息密码错误
    QObject::connect(&mysql_connector,SIGNAL(user_password_check_error(QByteArray)),
                     &information_error,SLOT(user_password_check_error(QByteArray)));
    //                                  诉求信息插入错误
    QObject::connect(&mysql_connector,SIGNAL(appeal_info_insert_error(appeal_info)),
                     &information_error,SLOT(appeal_info_insert_error(appeal_info)));
    //                                  诉求信息已存在
    QObject::connect(&mysql_connector,SIGNAL(appeal_info_exist(appeal_info)),
                     &information_error,SLOT(appeal_info_exist(appeal_info)));

    //information_error--->json_parser  下行数据        err->json 失败
    //                                  用户注册错误
    QObject::connect(&information_error,SIGNAL(tell_account_exist()),
                     &json_parser,SLOT(create_packet_register_fail()));
    //                                  用户登录密码错误
    QObject::connect(&information_error,SIGNAL(tell_password_wrong(QByteArray)),
                     &json_parser,SLOT(create_packet_login_fail(QByteArray)));
    //                                  用户登录账号不存在
    QObject::connect(&information_error,SIGNAL(tell_account_not_exist(QByteArray)),
                     &json_parser,SLOT(create_packet_login_fail(QByteArray)));

    //                                  诉求信息已存在
    QObject::connect(&information_error,SIGNAL(tell_appeal_exist(QByteArray)),
                     &json_parser,SLOT(create_packet_appeal_fail(QByteArray)));

    //json_parser--->local_socket       下行数据        json->socket
    QObject::connect(&json_parser,SIGNAL(post_message_to_localsocket(QJsonObject)),
                     &local_socket,SLOT(get_message_from_jsonparser(QJsonObject)));

    local_socket.connect_to_server(server_port);
    return a.exec();
}
