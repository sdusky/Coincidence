#ifndef COMMON_H
#define COMMON_H
#include <QString>
#include <cstdlib>
#define DEBUG 1
struct user_info{
    QByteArray user_id;
    QByteArray user_account;
    QByteArray user_password;
    QByteArray user_name;
    QByteArray user_phonenumber;
    QByteArray user_company;
    QByteArray user_sexual;
    QByteArray user_verifycode;
};

struct appeal_info{
    QByteArray user_account;
    QByteArray issue_time;
    QByteArray appeal_date;
    QByteArray appeal_time;
    QByteArray appeal_city;
    QByteArray appeal_area;
    QByteArray appeal_location;
    QByteArray appeal_thing;
    QByteArray appeal_remark;
    QByteArray appeal_sexual;
    QByteArray appeal_status;
    user_info  user_information;
    int        appeal_id;
};

struct partner{
    appeal_info user1;
    appeal_info user2;
};

struct success_info{
    QByteArray user_account1;
    QByteArray user_account2;
    appeal_info appeal_information;
};

const QString DataBaseName = "coincidence";
const QString HostName = "localhost";
const QString Password = "Password123456789";
const QString UserName = "root";

const int server_port = 10005;

const int REGISTER    = 0;
const int LOGIN       = 1;




#endif // COMMON_H
