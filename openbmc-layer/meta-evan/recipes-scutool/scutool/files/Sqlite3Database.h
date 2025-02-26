#ifndef SQLITE3_DATABASE_H
#define SQLITE3_DATABASE_H

#include "./DataBaseAPI.h"
#include <sqlite3.h>  // 引入SQLite3库
#include <stdexcept> // 引入异常处理

class Sqlite3Database : public StdDataBase
{
public:
    Sqlite3Database();
    ~Sqlite3Database() override;

    // 实现纯虚函数
    bool connectToDatabase(const std::string &dbFileName) override;
    bool executeSql(const std::string &sql) override;
    VecResult querySql(const std::string &sql) override;
    void closeDatabaseConnection() override;

private:
    sqlite3 *db; // SQLite3数据库连接指针
};

#endif // SQLITE3_DATABASE_H