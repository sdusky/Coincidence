#include "jsonparser.h"

JsonParser::JsonParser(QObject *parent) :
    QObject(parent)
{
    map.insert("register"       ,   0);
    map.insert("login"          ,   1);
    map.insert("direct_login"   ,   2);
    map.insert("appeal"         ,   3);
    map.insert("next"           ,   4);
    map.insert("talking"        ,   5);
    map.insert("touch"          ,   6);
    map.insert("happened"       ,   7);
    map.insert("no_happened"    ,   8);
    map.insert("userinfo"       ,   9);

    qDebug("JsonParser object initialized!");
}
//获取服务端数据
void JsonParser::get_message_from_localsocket(QByteArray json_byte)
{
    QJsonParseError json_err;
    QJsonDocument   json_doc = QJsonDocument::fromJson(json_byte,&json_err);

    if(json_err.error != QJsonParseError::NoError)
    {
        emit json_parse_error(json_err.errorString());
        return;
    }

    if(json_doc.isObject())
    {
        QJsonObject json_obj = json_doc.object();
        if(json_obj.contains("flag"))
        {
            QString flag = json_obj.take("flag").toString();

            switch(map.value(flag))
            {
            case 0:user_register(json_obj)      ;break;
            case 1:user_login(json_obj)         ;break;
            case 2:user_login(json_obj)         ;break;
            case 3:appeal_issue(json_obj)       ;break;
            case 4:appeal_next(json_obj)        ;break;
            case 5:
            case 6:touch_to_happen(json_obj)    ;break;
            case 7:appeal_happen(json_obj)      ;break;
            case 8:appeal_no_happen(json_obj)   ;break;
            case 9:user_informtion(json_obj)    ;break;
            default:                             break;
            }
        }
    }
}
//解析用户信息
void JsonParser::get_user_info(user_info *info,QJsonObject json_obj)
{
    info->user_account       = json_obj.take("nickname").toString().toLatin1();
    info->user_name          = json_obj.take("username").toString().toLatin1();
    info->user_phonenumber   = json_obj.take("phone").toString().toLatin1();
    info->user_password      = json_obj.take("password").toString().toLatin1();
    info->user_sexual        = json_obj.take("gender").toString().toLatin1();
    info->user_verifycode    = json_obj.take("code").toString().toLatin1();
    info->user_company       = "360";                            //default

#if DEBUG
    qDebug()<<"----------user_info---------------";
    qDebug()<<"user_account:    "<<info->user_account;
    qDebug()<<"user_name   :    "<<info->user_name;
    qDebug()<<"user_phone  :    "<<info->user_phonenumber;
    qDebug()<<"user_sexual :    "<<info->user_sexual;
#endif
}

//获取诉求信息
void JsonParser::get_appeal_info(appeal_info *info, QJsonObject json_obj)
{
    info->user_account      = json_obj.take("nickname").toString().toLatin1();
    info->appeal_date       = json_obj.take("date").toString().toLatin1();
    info->appeal_time       = json_obj.take("time").toString().toLatin1();
    info->appeal_thing      = json_obj.take("event").toString().toLatin1();
    info->appeal_sexual     = json_obj.take("appeal_gender").toString().toLatin1();
    info->appeal_location   = json_obj.take("location").toString().toLatin1();
    info->appeal_remark     = json_obj.take("remarks").toString().toLatin1();
    info->issue_time        = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toLatin1();
    info->appeal_city       = "BeiJing";                  //default
    info->appeal_status     = "0";

#if DEBUG
    qDebug()<<"----------appeal_info---------------";
    qDebug()<<"user_account     :    "<<info->user_account;
    qDebug()<<"appeal_date      :    "<<info->appeal_date;
    qDebug()<<"appeal_time      :    "<<info->appeal_time;
    qDebug()<<"appeal_localtion :    "<<info->appeal_location;
    qDebug()<<"appeal_thing     :    "<<info->appeal_thing;
    qDebug()<<"appeal_remark    :    "<<info->appeal_remark;
    qDebug()<<"appeal_sexual    :    "<<info->appeal_sexual;
#endif

}

void JsonParser::get_partner_info(partner *info,QJsonObject json_obj)
{
    info->user1.user_account = json_obj.take("nickname").toString().toLatin1();
    info->user2.user_account = json_obj.take("touch_nickname").toString().toLatin1();
#if DEBUG
    qDebug()<<"----------partner_info---------------";
    qDebug()<<"user1.user_account:  "<<info->user1.user_account;
    qDebug()<<"user2.user_account:  "<<info->user2.user_account;
#endif
}

//0.用户注册
void JsonParser::user_register(QJsonObject json_obj)
{
    user_info info;
    get_user_info(&info,json_obj);

    emit user_register_to_mysqlconnector(info);
}
//1.用户登录 2.用户登录（7.23会议删除该流程）
void JsonParser::user_login(QJsonObject json_obj)
{
    user_info info;
    get_user_info(&info,json_obj);

    emit user_login_to_mysqlconnector(info);
}

//3.诉求发布
void JsonParser::appeal_issue(QJsonObject json_obj)
{
    appeal_info info;
    get_appeal_info(&info,json_obj);

    emit appeal_issue_to_mysqlconnector(info);
}
//4.下一个同伴
void JsonParser::appeal_next(QJsonObject json_obj)
{
    appeal_info info;
    get_appeal_info(&info,json_obj);

    emit appeal_next_to_mysqlconnector(info);
}

//6.碰一下
void JsonParser::touch_to_happen(QJsonObject json_obj)
{
    partner info;
    get_partner_info(&info,json_obj);

    emit touch_to_happen_to_mysqlconnector(info);
}

//7.碰巧
void JsonParser::appeal_happen(QJsonObject json_obj)
{
    partner info;
    get_partner_info(&info,json_obj);

    emit appeal_happen_to_mysqlconnector(info);
}

//8.不巧
void JsonParser::appeal_no_happen(QJsonObject json_obj)
{
    partner info;
    get_partner_info(&info,json_obj);

    emit appeal_no_happen_to_mysqlconnector(info);
}

//9.用户信息
void JsonParser::user_informtion(QJsonObject json_obj)
{
    user_info info;
    get_user_info(&info,json_obj);

    emit user_information_to_mysqlconnector(info);
}

//数据包生成：注册失败
void JsonParser::create_packet_register_fail()
{
    QJsonObject json_obj;

    json_obj.insert("flag"  ,   QString("register_fail"));
    json_obj.insert("mes"   ,   QString("user_account_exist!"));

    emit post_message_to_localsocket(json_obj);
}

//数据包生成：注册成功
void JsonParser::create_packet_register_success(QByteArray user_name)
{
    QJsonObject json_obj;

    json_obj.insert("flag"      ,   QString("register_ok"));
    json_obj.insert("nickname"  ,   QString(user_name));

    emit post_message_to_localsocket(json_obj);
}

//数据包生成：登录成功
void JsonParser::create_packet_login_success(QByteArray user_name)
{
    QJsonObject json_obj;

    json_obj.insert("flag"      ,   QString("login_ok"));
    json_obj.insert("nickname"  ,   QString(user_name));

    //TODO 添加当前已配对成功的信息，需要数据库造数据
    emit post_message_to_localsocket(json_obj);
}

//数据包生成：登录失败
void JsonParser::create_packet_login_fail(QByteArray err)
{
    QJsonObject json_obj;

    json_obj.insert("flag"      ,   QString("login_fail"));
    json_obj.insert("mes"       ,   QString(err));

    emit post_message_to_localsocket(json_obj);
}

//数据包生成：诉求发布成功
void JsonParser::create_packet_appeal_success(appeal_info info)
{
    QJsonObject json_obj;

    json_obj.insert("flag"       ,   QString("appeal_ok"));
    json_obj.insert("nickname"   ,   QString(info.user_account));
    json_obj.insert("date"       ,   QString(info.appeal_date));
    json_obj.insert("time"       ,   QString(info.appeal_time));
    json_obj.insert("city"       ,   QString(info.appeal_city));
    json_obj.insert("area"       ,   QString(info.appeal_area));
    json_obj.insert("location"   ,   QString(info.appeal_location));
    json_obj.insert("event"      ,   QString(info.appeal_thing));

    emit post_message_to_localsocket(json_obj);
}

//数据包生成：诉求发布失败
void JsonParser::create_packet_appeal_fail(QByteArray err)
{
    QJsonObject json_obj;

    json_obj.insert("flag"       ,   QString("appeal_fail"));
    json_obj.insert("mes"        ,   QString(err));

    emit post_message_to_localsocket(json_obj);
}

//数据包生成：推送用户信息,一对，生成两个数据包
void JsonParser::create_packet_appeal_post(partner user)
{
    QJsonObject json_obj;
    if(user.user1.appeal_remark == "")
    {
        user.user1.appeal_remark.append("-");
    }

    if(user.user2.appeal_remark == "")
    {
        user.user2.appeal_remark.append("-");
    }

    json_obj.insert("flag"       ,   QString("appeal_post"));
    json_obj.insert("nickname"   ,   QString(user.user1.user_account));
    json_obj.insert("partner"    ,   QString(user.user2.user_account));
    json_obj.insert("remarks"    ,   QString(user.user2.appeal_remark));

    QJsonObject json_obj_partner;

    json_obj_partner.insert("flag"       ,   QString("appeal_post"));
    json_obj_partner.insert("nickname"   ,   QString(user.user2.user_account));
    json_obj_partner.insert("partner"    ,   QString(user.user1.user_account));
    json_obj_partner.insert("remarks"    ,   QString(user.user1.appeal_remark));

    emit post_message_to_localsocket(json_obj);
    emit post_message_to_localsocket(json_obj_partner);
}

//数据包生成：碰一下反馈
void JsonParser::create_packet_touch(partner info)
{
    QJsonObject json_obj;

    json_obj.insert("flag"              ,   QString("touch_ok"));
    json_obj.insert("nickname"          ,   QString(info.user1.user_account));
    json_obj.insert("touch_nickname"    ,   QString(info.user2.user_account));

    emit post_message_to_localsocket(json_obj);
}

void JsonParser::create_packet_touch_fail(partner info)
{
    QJsonObject json_obj;

    json_obj.insert("flag"              ,   QString("touch_fail"));
    json_obj.insert("nickname"          ,   QString(info.user1.user_account));
    json_obj.insert("touch_nickname"    ,   QString(info.user2.user_account));

    QJsonObject json_obj_partner;

    json_obj_partner.insert("flag"              ,   QString("touch_fail"));
    json_obj_partner.insert("nickname"          ,   QString(info.user2.user_account));
    json_obj_partner.insert("touch_nickname"    ,   QString(info.user1.user_account));

    emit post_message_to_localsocket(json_obj);
    emit post_message_to_localsocket(json_obj_partner);
}

//数据包生成：碰巧反馈
void JsonParser::create_packet_happen(partner info)
{
    QJsonObject json_obj;

    json_obj.insert("flag"              ,   QString("happen_ok"));
    json_obj.insert("nickname"          ,   QString(info.user1.user_account));
    json_obj.insert("touch_nickname"    ,   QString(info.user2.user_account));

    emit post_message_to_localsocket(json_obj);
}
//数据包生成：不巧反馈
void JsonParser::create_packet_no_happen(partner info)
{
    QJsonObject json_obj;

    json_obj.insert("flag"              ,   QString("happen_fail"));
    json_obj.insert("nickname"          ,   QString(info.user1.user_account));
    json_obj.insert("touch_nickname"    ,   QString(info.user2.user_account));

    emit post_message_to_localsocket(json_obj);
}

//数据包生成：发送用户信息
void JsonParser::create_packet_information(partner info)
{
    QJsonObject json_obj;

    json_obj.insert("flag"              ,   QString("userinfo_ok"));
    json_obj.insert("nickname"          ,   QString(info.user1.user_account));
    json_obj.insert("touch_nickname"    ,   QString(info.user2.user_account));
    json_obj.insert("touch_phone"       ,   QString(info.user2.user_information.user_phonenumber));

    QJsonObject json_obj_partner;

    json_obj_partner.insert("flag"              ,   QString("userinfo_ok"));
    json_obj_partner.insert("nickname"          ,   QString(info.user2.user_account));
    json_obj_partner.insert("touch_nickname"    ,   QString(info.user1.user_account));
    json_obj_partner.insert("touch_phone"       ,   QString(info.user1.user_information.user_phonenumber));

    emit post_message_to_localsocket(json_obj);
    emit post_message_to_localsocket(json_obj_partner);
}
//数据包生成：接收自己信息
void JsonParser::create_packet_personal_information(user_info info)
{
    QJsonObject json_obj;

    json_obj.insert("flag"              ,   QString("userinfo_ok"));
    json_obj.insert("nickname"          ,   QString(info.user_account));
    json_obj.insert("touch_nickname"    ,   QString(info.user_account));
    json_obj.insert("touch_phone"       ,   QString(info.user_phonenumber));

    emit post_message_to_localsocket(json_obj);
}

//数据包生成：允许聊天
void JsonParser::create_packet_start_chat(partner user)
{
    QJsonObject json_obj;

    json_obj.insert("flag"              ,   QString("chat"));
    if(user.user1.user_account == "")
    {
        user.user1.user_account = "-";
    }
    if(user.user2.user_account == "")
    {
        user.user2.user_account = "-";
    }
    json_obj.insert("nickname"          ,   QString(user.user1.user_account));
    json_obj.insert("touch_nickname"    ,   QString(user.user2.user_account));

    emit post_message_to_localsocket(json_obj);
}
