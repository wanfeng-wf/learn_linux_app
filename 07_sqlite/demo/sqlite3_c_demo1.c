#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

int main(int argc, char *argv[])
{
    sqlite3 *db   = NULL;      // 数据库句柄（嵌入式中建议设为静态/全局）
    char *err_msg = NULL;      // 错误信息缓冲区（需手动释放）
    int ret       = SQLITE_OK; // SQLite函数返回值

    // 1. 打开/创建数据库（路径和命令行的embedded_db.db一致）
    ret = sqlite3_open("embedded_db.db", &db);
    if (ret != SQLITE_OK)
    {
        fprintf(stderr, "[ERROR] 打开数据库失败: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db); // 即使失败也要尝试关闭，避免资源泄漏
        return -1;
    }
    printf("[INFO] 数据库打开/创建成功\n");

    // 2. 定义创建表的SQL语句（和命令行的表结构完全一致）
    const char *create_table_sql = "CREATE TABLE IF NOT EXISTS device_params ("
                                   "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                                   "param_name TEXT NOT NULL, "
                                   "param_value REAL NOT NULL, "
                                   "update_ts TIMESTAMP DEFAULT CURRENT_TIMESTAMP);";

    // 3. 执行创建表的SQL语句（callback=NULL，arg=NULL）
    ret = sqlite3_exec(db, create_table_sql, NULL, NULL, &err_msg);
    if (ret != SQLITE_OK)
    {
        fprintf(stderr, "[ERROR] 创建表失败: %s\n", err_msg);
        sqlite3_free(err_msg); // 释放错误信息缓冲区
        sqlite3_close(db);
        return -1;
    }
    printf("[INFO] 表创建成功（已存在则忽略）\n");

    // 4. 关闭数据库
    ret = sqlite3_close(db);
    if (ret != SQLITE_OK)
    {
        fprintf(stderr, "[ERROR] 关闭数据库失败: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    printf("[INFO] 数据库关闭成功\n");

    return 0;
}