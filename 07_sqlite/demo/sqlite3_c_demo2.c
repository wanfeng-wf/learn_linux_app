#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

// 回调函数：处理SELECT查询结果（可改为存储到全局数组/结构体）
// 每查询到一行数据，该函数就会被调用一次
static int select_callback(void *data, int argc, char **argv, char **azColName)
{
    // data：用户传入的自定义提示信息（sqlite3_exec的第4个参数）
    printf("[%s]\n", (char *)data);

    // 遍历每一列，打印列名+值（可解析为对应类型，如param_value转float）
    for (int i = 0; i < argc; i++)
    {
        // argv[i]可能为NULL（字段为空），需做判空处理
        printf("  %s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");

    return 0; // 返回0：继续处理下一行；返回非0：终止查询
}

int main(int argc, char *argv[])
{
    sqlite3 *db   = NULL;
    char *err_msg = NULL;
    int ret       = SQLITE_OK;

    // 1. 打开数据库
    ret = sqlite3_open("embedded_db.db", &db);
    if (ret != SQLITE_OK)
    {
        fprintf(stderr, "[ERROR] 打开数据库失败: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return -1;
    }
    printf("[INFO] 数据库打开成功\n");

    // 2. 插入数据（模拟传感器采集的温度/湿度数据）
    const char *insert_sql = "INSERT INTO device_params (param_name, param_value, update_ts) VALUES "
                             "('temp', 26.5, datetime(CURRENT_TIMESTAMP, '+8 hours')), "
                             "('humidity', 61.0, datetime(CURRENT_TIMESTAMP, '+8 hours'));";
    ret                    = sqlite3_exec(db, insert_sql, NULL, NULL, &err_msg);
    if (ret != SQLITE_OK)
    {
        fprintf(stderr, "[ERROR] 插入数据失败: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return -1;
    }
    printf("[INFO] 数据插入成功\n");

    // 3. 查询所有数据（调用回调函数处理结果）
    const char *select_all_sql = "SELECT * FROM device_params;";
    ret                        = sqlite3_exec(db, select_all_sql, select_callback, (void *)"查询所有设备参数", &err_msg);
    if (ret != SQLITE_OK)
    {
        fprintf(stderr, "[ERROR] 查询所有数据失败: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return -1;
    }

    // 4. 条件查询
    const char *select_temp_sql = "SELECT param_name, param_value FROM device_params WHERE param_name = 'temp';";
    ret                         = sqlite3_exec(db, select_temp_sql, select_callback, (void *)"查询温度参数", &err_msg);
    if (ret != SQLITE_OK)
    {
        fprintf(stderr, "[ERROR] 查询温度失败: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return -1;
    }

    // 5. 关闭数据库
    ret = sqlite3_close(db);
    if (ret != SQLITE_OK)
    {
        fprintf(stderr, "[ERROR] 关闭数据库失败: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    printf("[INFO] 数据库关闭成功\n");

    return 0;
}