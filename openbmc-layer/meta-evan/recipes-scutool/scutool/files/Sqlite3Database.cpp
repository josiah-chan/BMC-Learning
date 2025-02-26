#include "./Sqlite3Database.h"

Sqlite3Database::Sqlite3Database() : db(nullptr) {}

Sqlite3Database::~Sqlite3Database()
{
    if (db != nullptr)
    {
        sqlite3_close(db);
    }
}

// 连接数据库
bool Sqlite3Database::connectToDatabase(const std::string &dbFileName)
{
    if (sqlite3_open(dbFileName.c_str(), &db) == SQLITE_OK)
    {
        return true;
    }
    else
    {
        throw std::runtime_error("Cannot open database: " + std::string(sqlite3_errmsg(db)));
        return false;
    }
}

bool Sqlite3Database::executeSql(const std::string &sql)
{
    char *errMsg = nullptr;
    if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK)
    {
        throw std::runtime_error("SQL error: " + std::string(errMsg));
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

// 查询SQL语句
VecResult Sqlite3Database::querySql(const std::string &sql)
{
    VecResult result;
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
    {
        int cols = sqlite3_column_count(stmt);
        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            std::vector<std::string> row;
            for (int i = 0; i < cols; ++i)
            {
                row.push_back(std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, i))));
            }
            result.push_back(row);
        }
        sqlite3_finalize(stmt);
    }
    else
    {
        throw std::runtime_error("Query failed: " + std::string(sqlite3_errmsg(db)));
    }
    return result;
}

// 关闭数据库连接
void Sqlite3Database::closeDatabaseConnection()
{
    if (db != nullptr)
    {
        sqlite3_close(db);
        db = nullptr;
    }
}