#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

#define DATABASE   "student.db"
#define TABLE_NAME "stu"
#define N          128

typedef struct
{
    int id;
    int score;
    char sex[4];
    char name[16];
} Student;

static int do_insert(sqlite3 *db, char *table_name);
static int do_delete(sqlite3 *db, char *table_name);
static int do_update(sqlite3 *db, char *table_name);
static int do_query(sqlite3 *db, char *table_name);

int main(int argc, const char *argv[])
{
    sqlite3 *db   = NULL;
    char *err_msg = NULL;
    int ret       = SQLITE_OK;
    int n         = 0;

    ret = sqlite3_open(DATABASE, &db);
    if (ret != SQLITE_OK)
    {
        fprintf(stderr, "[ERROR] 打开数据库失败: %s\n", sqlite3_errmsg(db));
        if (db) {
            sqlite3_close(db);
        }
        return -1;
    }
    printf("[INFO] 打开数据库成功\n");

    char create_table_sql[N] = {};
    sprintf(create_table_sql, "CREATE TABLE IF NOT EXISTS %s(id INTEGER, name TEXT, sex TEXT, score int);", TABLE_NAME);
    ret = sqlite3_exec(db, create_table_sql, NULL, NULL, &err_msg);
    if (ret != SQLITE_OK)
    {
        fprintf(stderr, "[ERROR] 创建或打开表失败: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return -1;
    }
    printf("[INFO] 创建或打开表成功\n");

    while (1)
    {
        printf("\n-------------------------------------------------------\n");
        printf("1: 插入数据 2:查询数据 3:删除数据 4:更新数据 5:退出程序\n");
        printf("-------------------------------------------------------\n");
        printf("请选择:");
        scanf("%d", &n);

        switch (n)
        {
            case 1:
                do_insert(db, TABLE_NAME);
                break;
            case 2:
                do_query(db, TABLE_NAME);
                break;
            case 3:
                do_delete(db, TABLE_NAME);
                break;
            case 4:
                do_update(db, TABLE_NAME);
                break;
            case 5:
                printf("[INFO] 退出程序\n");
                sqlite3_close(db);
                exit(0);
                break;
            default:
                fprintf(stderr, "[ERROR] 无效的选择: %d\n", n);
        }
    }

    return 0;
}

static int do_insert(sqlite3 *db, char *table_name)
{
    Student stu;
    sqlite3_stmt *stmt = NULL;
    int ret = 0;
    char sql[N] = {};

    printf("学号:");
    scanf("%d", &stu.id);
    if (stu.id <= 0) {
        fprintf(stderr, "[ERROR] 学号必须为正整数\n");
        return -1;
    }

    printf("姓名:");
    scanf("%15s", stu.name);

    printf("性别:");
    scanf("%3s", stu.sex);
    if (strcmp(stu.sex, "M") != 0 && strcmp(stu.sex, "F") != 0 && 
        strcmp(stu.sex, "男") != 0 && strcmp(stu.sex, "女") != 0) {
        fprintf(stderr, "[ERROR] 性别必须为M/F或男/女\n");
        return -1;
    }

    printf("成绩:");
    scanf("%d", &stu.score);
    if (stu.score < 0 || stu.score > 100) {
        fprintf(stderr, "[ERROR] 成绩必须在0-100之间\n");
        return -1;
    }

    sprintf(sql, "INSERT INTO %s (id, name, sex, score) VALUES (?, ?, ?, ?)", table_name);
    ret = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (ret != SQLITE_OK) {
        fprintf(stderr, "[ERROR] 预处理失败: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_bind_int(stmt, 1, stu.id);
    sqlite3_bind_text(stmt, 2, stu.name, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, stu.sex, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, stu.score);

    ret = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (ret != SQLITE_DONE) {
        fprintf(stderr, "[ERROR] 插入数据失败: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    printf("[INFO] 插入数据成功\n");
    return 0;
}

static int do_delete(sqlite3 *db, char *table_name)
{
    sqlite3_stmt *stmt = NULL;
    int ret = 0;
    Student stu;
    int n = 0;
    char sql[N] = {};

    printf("删除方式: 1.删除指定学号的数据 2.删除指定姓名的数据 3.删除所有数据\n");
    printf("请选择:");
    scanf("%d", &n);

    switch (n)
    {
        case 1:
            printf("学号:");
            scanf("%d", &stu.id);
            sprintf(sql, "DELETE FROM %s WHERE id = ?", table_name);
            ret = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
            if (ret != SQLITE_OK) break;
            sqlite3_bind_int(stmt, 1, stu.id);
            break;
        case 2:
            printf("姓名:");
            scanf("%15s", stu.name);
            sprintf(sql, "DELETE FROM %s WHERE name = ?", table_name);
            ret = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
            if (ret != SQLITE_OK) break;
            sqlite3_bind_text(stmt, 1, stu.name, -1, SQLITE_TRANSIENT);
            break;
        case 3:
            sprintf(sql, "DELETE FROM %s", table_name);
            ret = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
            if (ret != SQLITE_OK) break;
            break;
        default:
            fprintf(stderr, "[ERROR] 无效的选择: %d\n", n);
            return -1;
    }

    if (ret != SQLITE_OK) {
        fprintf(stderr, "[ERROR] 预处理失败: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    ret = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (ret != SQLITE_DONE) {
        fprintf(stderr, "[ERROR] 删除数据失败: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    printf("[INFO] 删除数据成功\n");

    return 0;
}

static int do_update(sqlite3 *db, char *table_name)
{
    Student stu;
    sqlite3_stmt *stmt = NULL;
    int ret = 0;
    int n = 0;
    char sql[N] = {};

    printf("更新方式: 1.更新指定学号的数据 2.更新指定姓名的数据\n");
    printf("请选择:");
    scanf("%d", &n);

    switch (n)
    {
        case 1:
            printf("学号:");
            scanf("%d", &stu.id);
            printf("请输入修改后的成绩:");
            scanf("%d", &stu.score);
            if (stu.score < 0 || stu.score > 100) {
                fprintf(stderr, "[ERROR] 成绩必须在0-100之间\n");
                return -1;
            }
            sprintf(sql, "UPDATE %s SET score = ? WHERE id = ?", table_name);
            ret = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
            if (ret != SQLITE_OK) break;
            sqlite3_bind_int(stmt, 1, stu.score);
            sqlite3_bind_int(stmt, 2, stu.id);
            break;
        case 2:
            printf("姓名:");
            scanf("%15s", stu.name);
            printf("请输入修改后的成绩:");
            scanf("%d", &stu.score);
            if (stu.score < 0 || stu.score > 100) {
                fprintf(stderr, "[ERROR] 成绩必须在0-100之间\n");
                return -1;
            }
            sprintf(sql, "UPDATE %s SET score = ? WHERE name = ?", table_name);
            ret = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
            if (ret != SQLITE_OK) break;
            sqlite3_bind_int(stmt, 1, stu.score);
            sqlite3_bind_text(stmt, 2, stu.name, -1, SQLITE_TRANSIENT);
            break;
        default:
            fprintf(stderr, "[ERROR] 无效的选择: %d\n", n);
            return -1;
    }

    if (ret != SQLITE_OK) {
        fprintf(stderr, "[ERROR] 预处理失败: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    ret = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (ret != SQLITE_DONE) {
        fprintf(stderr, "[ERROR] 更新数据失败: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    printf("[INFO] 更新数据成功\n");

    return 0;
}

static int do_query(sqlite3 *db, char *table_name)
{
    sqlite3_stmt *stmt = NULL;
    int ret = 0;
    Student stu;
    int n;
    char sql[N] = {};

    printf("查询方式: 1.查询所有数据 2.查询指定学号的数据 3.查询指定姓名的数据\n");
    printf("请选择:");
    scanf("%d", &n);

    switch (n)
    {
        case 1:
            sprintf(sql, "SELECT * FROM %s", table_name);
            break;
        case 2:
            printf("学号:");
            scanf("%d", &stu.id);
            sprintf(sql, "SELECT * FROM %s WHERE id = ?", table_name);
            break;
        case 3:
            printf("姓名:");
            scanf("%15s", stu.name);
            sprintf(sql, "SELECT * FROM %s WHERE name = ?", table_name);
            break;
        default:
            fprintf(stderr, "[ERROR] 无效的选择: %d\n", n);
            return -1;
    }

    ret = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (ret != SQLITE_OK) {
        fprintf(stderr, "[ERROR] 预处理失败: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    if (n == 2) {
        sqlite3_bind_int(stmt, 1, stu.id);
    } else if (n == 3) {
        sqlite3_bind_text(stmt, 1, stu.name, -1, SQLITE_TRANSIENT);
    }

    printf("查询结果如下:\n");
    int found = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        found = 1;
        printf("学号:%d\n", sqlite3_column_int(stmt, 0));
        printf("姓名:%s\n", sqlite3_column_text(stmt, 1));
        printf("性别:%s\n", sqlite3_column_text(stmt, 2));
        printf("成绩:%d\n", sqlite3_column_int(stmt, 3));
        printf("\n");
    }
    sqlite3_finalize(stmt);

    if (!found) {
        printf("[INFO] 无查询结果\n");
    } else {
        printf("[INFO] 查询数据成功\n");
    }

    return 0;
}
