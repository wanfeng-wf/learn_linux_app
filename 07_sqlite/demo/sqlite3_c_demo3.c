#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

// 回调函数：用于验证UPDATE/DELETE后的结果
static int select_callback(void *data, int argc, char **argv, char **azColName)
{
    printf("[%s]\n", (char *)data);
    for (int i = 0; i < argc; i++)
    {
        // 判空避免空指针
        printf("  %s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
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

    // 2. 更新数据（温度从26.5→27.0，模拟传感器新读数）
    const char *update_sql = "UPDATE device_params SET param_value = 27.0, update_ts = datetime(CURRENT_TIMESTAMP, '+8 hours') WHERE param_name = 'temp';";
    ret                    = sqlite3_exec(db, update_sql, NULL, NULL, &err_msg);
    if (ret != SQLITE_OK)
    {
        fprintf(stderr, "[ERROR] 更新温度失败: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return -1;
    }
    printf("[INFO] 温度更新成功（26.5→27.0）\n");

    // 3. 验证更新结果
    const char *select_temp_sql = "SELECT param_name, param_value FROM device_params WHERE param_name = 'temp';";
    ret                         = sqlite3_exec(db, select_temp_sql, select_callback, (void *)"更新后的温度参数", &err_msg);
    if (ret != SQLITE_OK)
    {
        fprintf(stderr, "[ERROR] 验证更新结果失败: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return -1;
    }

    // 4. 删除数据（清理湿度数据，模拟无效参数清理）
    const char *delete_sql = "DELETE FROM device_params WHERE param_name = 'humidity';";
    ret                    = sqlite3_exec(db, delete_sql, NULL, NULL, &err_msg);
    if (ret != SQLITE_OK)
    {
        fprintf(stderr, "[ERROR] 删除湿度数据失败: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return -1;
    }
    printf("[INFO] 湿度数据删除成功\n");

    // 5. 验证删除结果（查询所有数据，确认湿度记录消失）
    const char *select_all_sql = "SELECT * FROM device_params;";
    ret                        = sqlite3_exec(db, select_all_sql, select_callback, (void *)"删除后的所有参数", &err_msg);
    if (ret != SQLITE_OK)
    {
        fprintf(stderr, "[ERROR] 验证删除结果失败: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return -1;
    }

    // 6. 关闭数据库
    ret = sqlite3_close(db);
    if (ret != SQLITE_OK)
    {
        fprintf(stderr, "[ERROR] 关闭数据库失败: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    printf("[INFO] 数据库关闭成功\n");

    return 0;
}