#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

// 统一错误打印
static void print_sqlite_error(sqlite3 *db, const char *tag, int rc)
{
    fprintf(stderr, "[ERROR] %s failed (rc=%d): %s\n", tag, rc, sqlite3_errmsg(db));
}

int main(int argc, char *argv[])
{
    sqlite3 *db = NULL;
    int rc      = SQLITE_OK;

    // 1) 打开数据库
    rc = sqlite3_open("embedded_db.db", &db);
    if (rc != SQLITE_OK)
    {
        print_sqlite_error(db, "sqlite3_open", rc);
        sqlite3_close(db);
        return -1;
    }
    printf("[INFO] 数据库打开成功\n");

    // 2) 建表
    const char *create_table_sql =
        "CREATE TABLE IF NOT EXISTS device_params ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "param_name TEXT NOT NULL, "
        "param_value REAL NOT NULL, "
        "update_ts TIMESTAMP DEFAULT CURRENT_TIMESTAMP);";

    // 建表这类无参数 SQL，用 prepare 也行；这里用 prepare 统一风格
    sqlite3_stmt *stmt_create = NULL;
    rc                        = sqlite3_prepare_v2(db, create_table_sql, -1, &stmt_create, NULL);
    if (rc != SQLITE_OK)
    {
        print_sqlite_error(db, "sqlite3_prepare_v2(create)", rc);
        sqlite3_close(db);
        return -1;
    }
    rc = sqlite3_step(stmt_create);
    if (rc != SQLITE_DONE)
    {
        print_sqlite_error(db, "sqlite3_step(create)", rc);
        sqlite3_finalize(stmt_create);
        sqlite3_close(db);
        return -1;
    }
    sqlite3_finalize(stmt_create);
    printf("[INFO] 表创建成功（已存在则忽略）\n");

    // 3) INSERT（带参数绑定）
    // 对应：INSERT INTO device_params(param_name, param_value, update_ts) VALUES (?, ?, datetime(...));
    const char *insert_sql =
        "INSERT INTO device_params (param_name, param_value, update_ts) "
        "VALUES (?, ?, datetime(CURRENT_TIMESTAMP, '+8 hours'));";

    sqlite3_stmt *stmt_insert = NULL;
    rc                        = sqlite3_prepare_v2(db, insert_sql, -1, &stmt_insert, NULL);
    if (rc != SQLITE_OK)
    {
        print_sqlite_error(db, "sqlite3_prepare_v2(insert)", rc);
        sqlite3_close(db);
        return -1;
    }

    // 模拟写两条：temp / humidity
    struct
    {
        const char *name;
        double value;
    } rows[] = {{"temp", 26.5}, {"humidity", 61.0}};

    for (int i = 0; i < 2; i++)
    {
        // bind idx 从 1 开始：第1个 ? 是 name，第2个 ? 是 value
        sqlite3_bind_text(stmt_insert, 1, rows[i].name, -1, SQLITE_TRANSIENT);
        sqlite3_bind_double(stmt_insert, 2, rows[i].value);

        rc = sqlite3_step(stmt_insert);
        if (rc != SQLITE_DONE)
        {
            print_sqlite_error(db, "sqlite3_step(insert)", rc);
            sqlite3_finalize(stmt_insert);
            sqlite3_close(db);
            return -1;
        }

        // 为下一次循环复用 stmt（关键：reset + clear）
        sqlite3_reset(stmt_insert);
        sqlite3_clear_bindings(stmt_insert);
    }
    sqlite3_finalize(stmt_insert);
    printf("[INFO] 数据插入成功（temp/humidity）\n");

    // 4) SELECT（查询所有数据：逐行取列）
    const char *select_all_sql    = "SELECT id, param_name, param_value, update_ts FROM device_params;";
    sqlite3_stmt *stmt_select_all = NULL;

    rc = sqlite3_prepare_v2(db, select_all_sql, -1, &stmt_select_all, NULL);
    if (rc != SQLITE_OK)
    {
        print_sqlite_error(db, "sqlite3_prepare_v2(select_all)", rc);
        sqlite3_close(db);
        return -1;
    }

    printf("[查询所有设备参数]\n");
    while ((rc = sqlite3_step(stmt_select_all)) == SQLITE_ROW)
    {
        int id                          = sqlite3_column_int(stmt_select_all, 0);
        const unsigned char *param_name = sqlite3_column_text(stmt_select_all, 1);
        double param_value              = sqlite3_column_double(stmt_select_all, 2);
        const unsigned char *update_ts  = sqlite3_column_text(stmt_select_all, 3);

        printf("  id=%d, param_name=%s, param_value=%.2f, update_ts=%s\n",
               id,
               param_name ? (const char *)param_name : "NULL",
               param_value,
               update_ts ? (const char *)update_ts : "NULL");
    }
    if (rc != SQLITE_DONE)
    {
        print_sqlite_error(db, "sqlite3_step(select_all)", rc);
        sqlite3_finalize(stmt_select_all);
        sqlite3_close(db);
        return -1;
    }
    sqlite3_finalize(stmt_select_all);

    // 5) 条件 SELECT（按 param_name = ?）
    const char *select_by_name_sql =
        "SELECT param_name, param_value FROM device_params WHERE param_name = ?;";

    sqlite3_stmt *stmt_select_one = NULL;
    rc                            = sqlite3_prepare_v2(db, select_by_name_sql, -1, &stmt_select_one, NULL);
    if (rc != SQLITE_OK)
    {
        print_sqlite_error(db, "sqlite3_prepare_v2(select_one)", rc);
        sqlite3_close(db);
        return -1;
    }

    sqlite3_bind_text(stmt_select_one, 1, "temp", -1, SQLITE_TRANSIENT);

    printf("[查询温度参数]\n");
    while ((rc = sqlite3_step(stmt_select_one)) == SQLITE_ROW)
    {
        const unsigned char *name = sqlite3_column_text(stmt_select_one, 0);
        double value              = sqlite3_column_double(stmt_select_one, 1);
        printf("  param_name=%s, param_value=%.2f\n", name ? (const char *)name : "NULL", value);
    }
    if (rc != SQLITE_DONE)
    {
        print_sqlite_error(db, "sqlite3_step(select_one)", rc);
        sqlite3_finalize(stmt_select_one);
        sqlite3_close(db);
        return -1;
    }
    sqlite3_finalize(stmt_select_one);

    // 6) UPDATE（把 temp 改成 27.0，WHERE param_name = ?）
    const char *update_sql =
        "UPDATE device_params "
        "SET param_value = ?, update_ts = datetime(CURRENT_TIMESTAMP, '+8 hours') "
        "WHERE param_name = ?;";

    sqlite3_stmt *stmt_update = NULL;
    rc                        = sqlite3_prepare_v2(db, update_sql, -1, &stmt_update, NULL);
    if (rc != SQLITE_OK)
    {
        print_sqlite_error(db, "sqlite3_prepare_v2(update)", rc);
        sqlite3_close(db);
        return -1;
    }

    sqlite3_bind_double(stmt_update, 1, 27.0);
    sqlite3_bind_text(stmt_update, 2, "temp", -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt_update);
    if (rc != SQLITE_DONE)
    {
        print_sqlite_error(db, "sqlite3_step(update)", rc);
        sqlite3_finalize(stmt_update);
        sqlite3_close(db);
        return -1;
    }
    sqlite3_finalize(stmt_update);
    printf("[INFO] 温度更新成功（temp -> 27.0）\n");

    // 7) DELETE（删除 humidity，WHERE param_name = ?）
    const char *delete_sql    = "DELETE FROM device_params WHERE param_name = ?;";
    sqlite3_stmt *stmt_delete = NULL;

    rc = sqlite3_prepare_v2(db, delete_sql, -1, &stmt_delete, NULL);
    if (rc != SQLITE_OK)
    {
        print_sqlite_error(db, "sqlite3_prepare_v2(delete)", rc);
        sqlite3_close(db);
        return -1;
    }

    sqlite3_bind_text(stmt_delete, 1, "humidity", -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt_delete);
    if (rc != SQLITE_DONE)
    {
        print_sqlite_error(db, "sqlite3_step(delete)", rc);
        sqlite3_finalize(stmt_delete);
        sqlite3_close(db);
        return -1;
    }
    sqlite3_finalize(stmt_delete);
    printf("[INFO] 湿度数据删除成功（humidity）\n");

    // 8) 关闭数据库
    rc = sqlite3_close(db);
    if (rc != SQLITE_OK)
    {
        print_sqlite_error(db, "sqlite3_close", rc);
        return -1;
    }
    printf("[INFO] 数据库关闭成功\n");

    return 0;
}