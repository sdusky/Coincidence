#include "mysqlconnector.h"
#include <QDebug>
MySQLConnector::MySQLConnector(QObject *parent) :
    QObject(parent)
{
    qDebug("MySQLConnector object initialized!");
    if(!connect_to_mysql())
    {
        emit connect_to_db_error("Connect to database error! Please check your database!");
        return;
    }
    else
    {
        match_timer = new QTimer;
        connect(match_timer,SIGNAL(timeout()),this,SLOT(find_partner()));
        match_timer->start(10000);
        qDebug("Connect to mysql!");
    }

}

void MySQLConnector::find_partner()
{
    QSqlQuery find_query;
    appeal_info info;
    QString sql_find;

    find_query.exec("SELECT *FROM where appealStatus = '0'");
    if(find_query.size() <= 0)
    {
        return;
    }
    find_query.next();
    info.user_account.append(find_query.value(1).toString());
    info.user_sexual.append(find_query.value(2).toString());
    info.appeal_date.append(find_query.value(4).toString());
    info.appeal_time.append(find_query.value(5).toString());
    info.appeal_city.append(find_query.value(6).toString());
    info.appeal_area.append(find_query.value(7).toString());
    info.appeal_location.append(find_query.value(8).toString());
    info.appeal_thing.append(find_query.value(9).toString());
    info.appeal_remark.append(find_query.value(10).toString());
    info.appeal_sexual.append(find_query.value(11).toString());

    appeal_information_search(info);

}

bool MySQLConnector::connect_to_mysql()
{
    QSqlDatabase mysql_db = QSqlDatabase::addDatabase("QMYSQL");

    mysql_db.setDatabaseName(DataBaseName);
    mysql_db.setHostName(HostName);
    mysql_db.setPassword(Password);
    mysql_db.setUserName(UserName);

    return mysql_db.open();
}

bool MySQLConnector::user_register_insert(user_info user)
{
    QSqlQuery insert_query;

    //拼接插入字符串 用户账号，用户密码，用户姓名，电话号码，公司，性别，验证码
    QString sql_insert;
    sql_insert.clear();
    sql_insert.append("INSERT INTO user (userAccount,passWord,userName,phoneNumber,company,sexual,verifyCode) ");
    sql_insert.append("VALUES(?,?,?,?,?,?,?)");

    insert_query.prepare(sql_insert);

    //绑定字段数据
    insert_query.addBindValue(user.user_account);
    insert_query.addBindValue(user.user_password);
    insert_query.addBindValue(user.user_name);
    insert_query.addBindValue(user.user_phonenumber);
    insert_query.addBindValue(user.user_company);
    insert_query.addBindValue(user.user_sexual);
    insert_query.addBindValue(user.user_verifycode);

    //执行insert操作
    if(!insert_query.exec())
    {
        //插入错误 发送错误信息：注册失败
        emit user_info_insert_error(user.user_account);
        return false;
    }

    //用户注册成功
    emit user_info_insert_success(user.user_account);
    return true;
}

bool MySQLConnector::user_login_check(user_info user)
{
    QSqlQuery check_query;

    //拼接查询数据块
    QString sql_check;
    sql_check.clear();
    sql_check.append("SELECT passWord FROM user WHERE userAccount = '");
    sql_check.append(user.user_account);
    sql_check.append("'");

    //执行查询操作
    check_query.exec(sql_check);

    //未找到账号
    if(check_query.size() < 1)
    {
        emit user_account_check_error(user.user_account);
        return false;
    }

    check_query.next();
    //账号密码不匹配
    if(check_query.value(0).operator !=(user.user_password))
    {
        emit user_password_check_error(user.user_account);
        return false;
    }
    QJsonArray json_array;
    get_success_information(&json_array);

    emit user_info_login_success(user.user_account,json_array);
    return true;
}

bool MySQLConnector::user_information_search(user_info info)
{
    QSqlQuery search_query;

    QString sql_search;
    sql_search.append("SELECT phoneNumber FROM user WHERE userAccount ='");
    sql_search.append(info.user_account);
    sql_search.append("'");

    search_query.exec(sql_search);

    if(search_query.size() < 0)
    {
        return false;
    }
    search_query.next();
    info.user_phonenumber.append(search_query.value(0).toString());
    emit send_personal_phone_to_jsonparser(info);
    return true;
}

int MySQLConnector::get_appeal_information_appealid(appeal_info appeal)
{
    QSqlQuery get_query;

    //查询该用户诉求ID
    QString sql_get;
    sql_get.clear();
    sql_get.append("SELECT appealID FROM appeal WHERE userAccount = '");
    sql_get.append(appeal.user_account);
    sql_get.append("' AND appealDate = '");
    sql_get.append(appeal.appeal_date);
    sql_get.append("' AND appealTime = '");
    sql_get.append(appeal.appeal_time);
    sql_get.append("' AND toDo ='");
    sql_get.append(appeal.appeal_thing);
    sql_get.append("'");

    get_query.exec(sql_get);

    if(get_query.size() > 0)
    {
        get_query.next();
        return get_query.value(0).toInt();
    }

    return -1;
}

bool MySQLConnector::appeal_information_check(appeal_info appeal)
{
    QSqlQuery check_query;

    //查询该用户是否在同一时间发布过诉求
    QString sql_check;
    sql_check.clear();
    sql_check.append("SELECT * FROM appeal WHERE userAccount = '");
    sql_check.append(appeal.user_account);
    sql_check.append("' and appealDate = '");
    sql_check.append(appeal.appeal_date);
    sql_check.append("' and appealTime = '");
    sql_check.append(appeal.appeal_time);
    sql_check.append("'");

    check_query.exec(sql_check);

    if(check_query.size() > 0)
    {
        emit appeal_info_exist(appeal);
        return false;
    }

    appeal_information_insert(appeal);
    return true;
}

bool MySQLConnector::get_sexual_by_nickname(QByteArray user_name, user_info *info)
{
    info->user_account = user_name;

    QSqlQuery get_query;

    QString sql_get;
    sql_get.append("SELECT sexual FROM user WHERE userAccount = '");
    sql_get.append(user_name);
    sql_get.append("'");

    get_query.exec(sql_get);

    if(get_query.size() <= 0)
    {
        return false;
    }

    get_query.next();

    info->user_sexual.append(get_query.value(0).toString());
    return true;
}

bool MySQLConnector::appeal_information_insert(appeal_info appeal)
{
    QSqlQuery insert_query;

    user_info info;
    get_sexual_by_nickname(appeal.user_account,&info);
    //拼接插入字符串 用户ID，发布时间，诉求日期，诉求时间，城市，区域，地点，事件，诉求性别，诉求状态
    ///诉求状态0为新发布，1为匹配
    QString sql_insert;
    sql_insert.clear();
    sql_insert.append("INSERT INTO appeal (userAccount,userSexual,issueTime,appealDate,appealTime,city,area,location,toDo,appealSexual,appealStatus) ");
    sql_insert.append("VALUES (?,?,?,?,?,?,?,?,?,?,?)");

    insert_query.prepare(sql_insert);

    //绑定字段数据
    insert_query.addBindValue(appeal.user_account);
    insert_query.addBindValue(info.user_sexual);
    insert_query.addBindValue(appeal.issue_time);
    insert_query.addBindValue(appeal.appeal_date);
    insert_query.addBindValue(appeal.appeal_time);
    insert_query.addBindValue(appeal.appeal_city);
    insert_query.addBindValue(appeal.appeal_area);
    insert_query.addBindValue(appeal.appeal_location);
    insert_query.addBindValue(appeal.appeal_thing);
    insert_query.addBindValue(appeal.appeal_sexual);
    insert_query.addBindValue(0);

    //执行insert操作
    if(!insert_query.exec())
    {
        qDebug()<<insert_query.lastError().text();
        //插入错误 发送错误信息
        emit appeal_info_insert_error(appeal);
        return false;
    }

    appeal_information_search(appeal);
    return true;
}

bool MySQLConnector::appeal_information_update(partner info,QString status)
{

#if DEBUG
    qDebug()<<"user1->appeal_id:"<<info.user1.appeal_id;
    qDebug()<<"user1->user_account"<<info.user1.user_account;
    qDebug()<<"user1->appeal_remark"<<info.user1.appeal_remark;
    qDebug()<<"user2->appeal_id:"<<info.user2.appeal_id;
    qDebug()<<"user2->user_account"<<info.user2.user_account;
    qDebug()<<"user2->appeal_remark"<<info.user2.appeal_remark;
#endif

    QSqlQuery update_query;

    QString sql_update;
    sql_update.clear();
    sql_update.append("UPDATE appeal SET appealStatus = '");
    sql_update.append(status);
    sql_update.append("' WHERE appealID = ");
    sql_update.append(QString::number(info.user1.appeal_id));
    //执行update操作
    if(!update_query.exec(sql_update))
    {

        //更新错误 发送错误信息
        emit appeal_info_update(update_query.lastError().text().toLatin1());
        return false;
    }

    sql_update.clear();
    sql_update.append("UPDATE appeal SET appealStatus = '");
    sql_update.append(status);
    sql_update.append("' WHERE appealID = ");
    sql_update.append(QString::number(info.user2.appeal_id));

    if(!update_query.exec(sql_update))
    {
        //更新错误 发送错误信息
        emit appeal_info_update(update_query.lastError().text().toLatin1());
        return false;
    }

    return true;
}

//查询符合条件用户
bool MySQLConnector::appeal_information_search(appeal_info appeal)
{
    QSqlQuery search_query;

    QString sql_search;
    sql_search.clear();
    if(appeal.appeal_sexual.toLower() != "all")
    {
        sql_search.append("SELECT * FROM appeal WHERE appealDate = '");
        sql_search.append(appeal.appeal_date);
        sql_search.append("' AND appealTime = '");
        sql_search.append(appeal.appeal_time);
        sql_search.append("' AND city = '");
        sql_search.append(appeal.appeal_city);
        sql_search.append("' AND location = '");
        sql_search.append(appeal.appeal_location);
        sql_search.append("' AND toDo = '");
        sql_search.append(appeal.appeal_thing);
        sql_search.append("' AND appealStatus = '");
        sql_search.append("0' AND userAccount <>'");
        sql_search.append(appeal.user_account);
        sql_search.append("' AND userSexual = '");
        sql_search.append(appeal.appeal_sexual);
        sql_search.append("'");
    }
    else
    {
        sql_search.append("SELECT * FROM appeal WHERE appealDate = '");
        sql_search.append(appeal.appeal_date);
        sql_search.append("' AND appealTime = '");
        sql_search.append(appeal.appeal_time);
        sql_search.append("' AND city = '");
        sql_search.append(appeal.appeal_city);
        sql_search.append("' AND location = '");
        sql_search.append(appeal.appeal_location);
        sql_search.append("' AND toDo = '");
        sql_search.append(appeal.appeal_thing);
        sql_search.append("' AND appealStatus = '");
        sql_search.append("0' AND userAccount <>'");
        sql_search.append(appeal.user_account);
        sql_search.append("'");
    }

    search_query.exec(sql_search);

    if(search_query.size() <= 0)
    {
        emit appeal_info_no_partner(appeal.user_account);
        return false;
    }
    QTime t;
    srand((unsigned int)t.msec());
    int number = rand() % (search_query.size());

    search_query.seek(number);

    //获取双方信息
    partner info;
    info.user1                  = appeal;
    info.user1.appeal_id = get_appeal_information_appealid(appeal);
    info.user2.appeal_id        = search_query.value(0).toString().toInt();
    info.user2.user_account.append(search_query.value(1).toString());
    info.user2.user_sexual.append(search_query.value(2).toString());
    info.user2.appeal_date.append(search_query.value(4).toString());
    info.user2.appeal_time.append(search_query.value(5).toString());
    info.user2.appeal_city.append(search_query.value(6).toString());
    info.user2.appeal_area.append(search_query.value(7).toString());
    info.user2.appeal_location.append(search_query.value(8).toString());
    info.user2.appeal_thing.append(search_query.value(9).toString());
    info.user2.appeal_remark.append(search_query.value(10).toString());
    info.user2.appeal_sexual.append(search_query.value(11).toString());

    //更新appeal表appealStatus字段
    appeal_information_update(info,"1");
    //插入success表
    success_information_insert(info);

    //向双方推送消息
    emit appeal_info_post_partner(info);
    return true;
}

void MySQLConnector::appeal_information_search(partner *info)
{
    QSqlQuery search_query;

    QString sql_search;
    sql_search.clear();
    sql_search.append("SELECT userAccount,appealDate,appealTime,location,toDo,remarks,appealSexual FROM appeal WHERE appealID = '");
    sql_search.append(QString::number(info->user1.appeal_id));
    sql_search.append("'");

    search_query.exec(sql_search);

    if(search_query.size() <= 0)
    {
        return;
    }

    search_query.next();
    info->user1.user_account.append(search_query.value(0).toString());
    info->user1.appeal_date.append(search_query.value(1).toString());
    info->user1.appeal_time.append(search_query.value(2).toString());
    info->user1.appeal_location.append(search_query.value(3).toString());
    info->user1.appeal_thing.append(search_query.value(4).toString());
    info->user1.appeal_remark.append(search_query.value(5).toString());
    info->user1.appeal_sexual.append(search_query.value(6).toString());

    sql_search.clear();
    sql_search.append("SELECT userAccount,appealDate,appealTime,location,toDo,remarks,appealSexual FROM appeal WHERE appealID = '");
    sql_search.append(QString::number(info->user2.appeal_id));
    sql_search.append("'");

    search_query.exec(sql_search);

    if(search_query.size() <= 0)
    {
        return;
    }

    search_query.next();
    info->user2.user_account.append(search_query.value(0).toString());
    info->user2.appeal_date.append(search_query.value(1).toString());
    info->user2.appeal_time.append(search_query.value(2).toString());
    info->user2.appeal_location.append(search_query.value(3).toString());
    info->user2.appeal_thing.append(search_query.value(4).toString());
    info->user2.appeal_remark.append(search_query.value(5).toString());
    info->user2.appeal_sexual.append(search_query.value(6).toString());
}

//
bool MySQLConnector::success_information_insert(partner success)
{
    QSqlQuery insert_query;

    QString sql_insert;
    sql_insert.clear();
    sql_insert.append("INSERT INTO successlist (userAccount1,userAccount2,appealID1,appealID2,appealStatus) ");
    sql_insert.append("VALUES (?,?,?,?,?)");

    insert_query.prepare(sql_insert);

    insert_query.addBindValue(success.user1.user_account);
    insert_query.addBindValue(success.user2.user_account);
    insert_query.addBindValue(success.user1.appeal_id);
    insert_query.addBindValue(success.user2.appeal_id);
    insert_query.addBindValue("0");

    insert_query.exec();

    return true;
}

void MySQLConnector::get_phone_number(partner * info)
{
    QSqlQuery need_query;

    QString sql_need;
    sql_need.clear();
    sql_need.append("SELECT phoneNumber FROM user WHERE userAccount ='");
    sql_need.append(info->user1.user_account);
    sql_need.append("'");

    need_query.exec(sql_need);
    need_query.next();
    info->user1.user_information.user_phonenumber.append(need_query.value(0).toString());


    sql_need.clear();
    sql_need.append("SELECT phoneNumber FROM user WHERE userAccount ='");
    sql_need.append(info->user2.user_account);
    sql_need.append("'");

    need_query.exec(sql_need);
    need_query.next();
    info->user2.user_information.user_phonenumber.append(need_query.value(0).toString());
}

/*
 * 单方面碰一下  置1
 * 双方都碰     置2
 * 单方面碰巧    置3
 * 双方都碰     置4
 *
 * 若出现下一个   status为-1删除记录
 * 不巧          status为-2删除记录
 *
 */
void MySQLConnector::success_information_update(partner info, QString status)
{
    if(status.toInt() == -1)
    {
        success_information_delete(info);
        appeal_information_search(info.user1);
        appeal_information_search(info.user2);
        return;
    }
    if(status.toInt() == -2)
    {
        success_information_delete(info);
        emit appeal_no_happen(info);
        return;
    }
    int now_status = success_information_check(info);
    if(now_status == 1 && status.toInt() == 1)
    {
        chat_start_signal(info);
        success_information_update(info,"2");
        return;
    }

    if(now_status == 3 && status.toInt() == 3)
    {
        //匹配成功
        get_phone_number(&info);
        emit appeal_happen(info);
        success_information_update(info,"4");
        return;
    }

    QSqlQuery update_query;

    QString sql_update;
    sql_update.clear();
    sql_update.append("UPDATE successlist SET appealStatus = '");
    sql_update.append(status);
    sql_update.append("' WHERE (userAccount1 = '");
    sql_update.append(info.user1.user_account);
    sql_update.append("' AND userAccount2 = '");
    sql_update.append(info.user2.user_account);
    sql_update.append("') OR (userAccount1 = '");
    sql_update.append(info.user2.user_account);
    sql_update.append("' AND userAccount2 = '");
    sql_update.append(info.user1.user_account);
    sql_update.append("')");

    update_query.exec(sql_update);

    emit touch_happen_to_jsonparser(info);
}

int MySQLConnector::success_information_check(partner info)
{
    QSqlQuery check_query;

    QString sql_check;
    sql_check.clear();
    sql_check.append("SELECT appealStatus FROM successlist WHERE (userAccount1 = '");
    sql_check.append(info.user1.user_account);
    sql_check.append("' AND userAccount2 = '");
    sql_check.append(info.user2.user_account);
    sql_check.append("') OR (userAccount1 = '");
    sql_check.append(info.user2.user_account);
    sql_check.append("' AND userAccount2 = '");
    sql_check.append(info.user1.user_account);
    sql_check.append("')");

    check_query.exec(sql_check);

    if(check_query.size() > 0)
    {
        check_query.next();
        return check_query.value(0).toInt();
    }

    return -1;
}

void MySQLConnector::success_information_search(partner *info)
{
    QSqlQuery search_query;

    QString sql_search;
    sql_search.clear();
    sql_search.append("SELECT appealID1,appealID2 FROM successlist WHERE (userAccount1 = '");
    sql_search.append(info->user1.user_account);
    sql_search.append("' OR userAccount2 = '");
    sql_search.append(info->user1.user_account);
    sql_search.append("') AND appealStatus != 4");

    search_query.exec(sql_search);

    if(search_query.size() <= 0)
    {
        return;
    }
    search_query.next();
    info->user1.appeal_id = search_query.value(0).toInt();
    info->user2.appeal_id = search_query.value(1).toInt();
}

void MySQLConnector::success_information_delete(partner info)
{
    QSqlQuery delete_query;

    QString sql_delete;
    sql_delete.clear();
    sql_delete.append("DELETE FROM successlist WHERE (userAccount1 = '");
    sql_delete.append(info.user1.user_account);
    sql_delete.append("' AND userAccount2 = '");
    sql_delete.append(info.user2.user_account);
    sql_delete.append("') OR (userAccount1 = '");
    sql_delete.append(info.user2.user_account);
    sql_delete.append("' AND userAccount2 = '");
    sql_delete.append(info.user1.user_account);
    sql_delete.append("')");

    delete_query.exec(sql_delete);

}

void MySQLConnector::get_success_information(QJsonArray * json_array)
{
    QSqlQuery search_query;

    QString sql_search;
    sql_search.append("SELECT appealID1,userAccount1,userAccount2 FROM successlist WHERE appealStatus = '4' ORDER BY successListID DESC LIMIT 3");

    search_query.exec(sql_search);

    if(search_query.size() <= 0)
    {
        return;
    }

    while(search_query.next())
    {
        QString appealID     = search_query.value(0).toString();
        QString userAccount1 = search_query.value(1).toString();
        QString userAccount2 = search_query.value(2).toString();

        QSqlQuery get_query;

        QString sql_get;
        sql_get.append("SELECT userSexual,appealSexual,appealDate,appealTime,location,toDo WHERE appealID = ");
        sql_get.append(appealID);

        get_query.exec(sql_get);

        if(get_query.size() < 0)
        {
            return;
        }
        int i = 0;
        while(get_query.next() && i < 3)
        {
            QJsonObject json_obj;

            json_obj.insert("nickname_1"    ,   userAccount1);
            json_obj.insert("gender_1"      ,   get_query.value(0).toString());
            json_obj.insert("nickname_2"    ,   userAccount2);
            json_obj.insert("gender_2"      ,   get_query.value(1).toString());
            json_obj.insert("date"          ,   get_query.value(2).toString());
            json_obj.insert("time"          ,   get_query.value(3).toString());
            json_obj.insert("location"      ,   get_query.value(4).toString());
            json_obj.insert("event"         ,   get_query.value(5).toString());

            json_array->insert(i++          ,   json_obj);
        }


    }
}

//用户注册处理函数：用户信息插入user表，如果出现异常则说明用户已存在
void MySQLConnector::get_user_register_from_jsonparser(user_info info)
{
    user_register_insert(info);
}
//用户登录处理函数：查询user表
void MySQLConnector::get_user_login_from_jsonparser(user_info info)
{
    user_login_check(info);
}

void MySQLConnector::get_appeal_issue_from_jsonparser(appeal_info info)
{
    appeal_information_check(info);
}
//下一个处理函数：重新查找符合条件用户
void MySQLConnector::get_appeal_next_from_jsonparser(appeal_info user)
{
    //获取双方信息
    partner info;
    info.user1 = user;
    success_information_search(&info);
    appeal_information_search(&info);
    emit touch_no_happen_to_jsonparser(info);

    //重置appeal表中appealStatus的值为0
    appeal_information_update(info,"0");
    //success表中记录删除
    success_information_update(info,"-1");   
}
//碰一下处理函数：更新表success
void MySQLConnector::get_touch_to_happen_from_jsonparser(partner info)
{
    success_information_update(info,"1");
}

void MySQLConnector::get_appeal_happen_from_jsonparser(partner info)
{
    success_information_update(info,"3");
}

void MySQLConnector::get_appeal_no_happen_from_jsonparser(partner info)
{
    success_information_update(info,"-2");
}

void MySQLConnector::get_user_information_from_jsonparser(user_info info)
{
    user_information_search(info);
}
