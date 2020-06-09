/**
 * KSM: simple MySQL encapsulation
 * 
 * 简单的 MySQL C++ 数据库操作层
 * 
 * @version 0.0.1
 * @author kirainmoe <kirainmoe@stu.xmu.edu.cn>
 */

#ifndef __KSM
#define __KSM

#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <map>

#include <mysql/mysql.h>

struct Tuple
{
    std::string key, value;
    Tuple(std::string _k, std::string _v): key(_k), value(_v) {}
};

struct MySQLResult
{
    bool flag;
    std::vector<std::map<std::string, std::string> > result;
};

class MySQL
{
private:
    std::string host;               // MySQL 服务器主机
    std::string user;               // MySQL 登录用户
    std::string pass;               // MySQL 登录密码
    std::string db_name;            // MySQL 数据库名
    unsigned int port;              // MySQL 端口

    std::string operation;
    std::string columns;
    std::string table;
    std::string filter;
    std::string limitation;
    std::vector<Tuple> fields;

public:
    MYSQL conn;

    MySQL(std::string _host, unsigned int _port, std::string _user, std::string _pass, std::string _dbname)
        : host(_host), port(_port), user(_user), pass(_pass), db_name(_dbname) {}

    /**
     * 设置使用的数据库
     * @return void
     */
    void setDatabase(std::string name);
    void useDatabase(std::string name);

    /**
     * 建立数据库连接
     * @return bool
     */
    bool connect();

    /**
     * 创建数据库
     * @return bool 返回创建结果
     */
    bool createDatabase();

    /**
     * 创建数据表
     * @param std::string tableName 表名
     * @param std::vector<Tuple> columns 表列类型
     * @param std::string primaryKey 主键
     * @return bool
     */
    bool createTable(std::string tableName, std::vector<Tuple> columns, std::string primaryKey);

    /**
     * SQL 转义防止注入
     * @param std::string original 转义前
     * @return std::string 返回转义后的结果
     */
    std::string escape(std::string original);

    MySQL* prepare();
    
    /**
     * SQL SELECT 查询
     * 
     * 用法：this->select({"*"})->from({"table"})->where({Tuple("id", "1"), Tuple("username", "root")})->limit("1")
     * 结果：SELECT * FROM table WHERE id = 1 AND username = root LIMIT 1
     */
    MySQL* select(std::vector<std::string> columns);
    MySQL* from(std::vector<std::string> columns);
    MySQL* where(std::vector<Tuple> where);
    MySQL* orWhere(std::vector<Tuple> where);
    MySQL* limit(std::string lim);

    /**
     * SQL INSERT 查询
     * 
     * 用法： this->insert("table")->values({Tuple("username", "test"), Tuple("password", "123456")})
     * 结果：INSERT INTO table (username, password) VALUES ("test", "123456")
     */
    MySQL* insert(std::string table);
    MySQL* values(std::vector<Tuple> vals);

    /**
     * SQL DELETE 查询
     * 
     * 用法：this->deletes("table")->where({ Tuple("username", "test"), Tuple("password", "123456") })
     * 结果：DELETE FROM table WHERE username = "test" AND "password" = "123456"
     */
    MySQL* deletes(std::string table);

    /**
     * SQL UPDATE 查询
     * 用法：this->update("table")->set({ Tuple("username", "root")  })->where({ Tuple("username", "admin") })
     * 结果：UPDATE table SET "username" = "root" WHERE "username" = "admin"
     */
    MySQL* update(std::string table);
    MySQL* set(std::vector<Tuple> vals);
    

    // 执行查询
    MySQLResult exec();

    /**
     * 自定义查询
     */
    void query(std::string);
};

#endif
