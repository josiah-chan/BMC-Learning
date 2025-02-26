#ifndef __DATA_BASE_API
#define __DATA_BASE_API

#include <string>
#include <vector>

using VecResult = std::vector<std::vector<std::string>>;

class StdDataBase
{
public:
    virtual ~StdDataBase() = default;

public:
    // 连接数据库
    virtual bool connectToDatabase(const std::string &dbFileName) = 0;

    // 执行SQL语句
    virtual bool executeSql(const std::string &sql) = 0;

    // 查询SQL语句
    virtual VecResult querySql(const std::string &sql) = 0;

    // 关闭数据库连接
    virtual void closeDatabaseConnection() = 0;
};

#endif // __DATA_BASE_API