#include <stdio.h>
#include <sqlite3.h>

int main()
{
    sqlite3 *db;
    char **result;
    int row;
    int col;
    int rc;
    char *errmsg;

    rc = sqlite3_open("embedded_db.db", &db);
    if (rc != SQLITE_OK)
    {
        printf("open db failed: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    const char *sql =
        "SELECT id,param_name,param_value,update_ts FROM device_params;";

    rc = sqlite3_get_table(db, sql, &result, &row, &col, &errmsg);

    if (rc != SQLITE_OK)
    {
        printf("query failed: %s\n", errmsg);
        sqlite3_free(errmsg);
        sqlite3_close(db);
        return -1;
    }

    printf("row=%d col=%d\n", row, col);

    /* 打印列名 */
    for (int i = 0; i < col; i++)
    {
        printf("%s\t", result[i]);
    }
    printf("\n");

    /* 打印数据 */
    for (int r = 0; r < row; r++)
    {
        for (int c = 0; c < col; c++)
        {
            printf("%s\t", result[(r + 1) * col + c]);
        }
        printf("\n");
    }

    /* 释放查询结果 */
    sqlite3_free_table(result);

    sqlite3_close(db);

    return 0;
}