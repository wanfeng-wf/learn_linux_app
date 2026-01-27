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
static int select_callback(void *data, int argc, char **argv, char **azColName);

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
        sqlite3_close(db);
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
    char sql[N] = {};
    char *errmsg;

    printf("学号:");
    scanf("%d", &stu.id);

    printf("姓名:");
    scanf("%s", stu.name);

    printf("性别:");
    scanf("%s", stu.sex);

    printf("成绩:");
    scanf("%d", &stu.score);

    sprintf(sql, "INSERT INTO %s (id, name, sex, score) VALUES (%d, '%s', '%s', %d)",
            table_name, stu.id, stu.name, stu.sex, stu.score);

    if (sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK)
    {
        fprintf(stderr, "[ERROR] 插入数据失败: %s\n", errmsg);
        sqlite3_free(errmsg);
        return -1;
    }
    else
    {
        printf("[INFO] 插入数据成功\n");
    }

    return 0;
}
static int do_delete(sqlite3 *db, char *table_name)
{
    char sql[N]  = {};
    int ret      = 0;
    char *errmsg = NULL;
    Student stu;
    int n = 0;

    printf("删除方式: 1.删除指定学号的数据 2.删除指定姓名的数据 3.删除所有数据\n");
    printf("请选择:");
    scanf("%d", &n);

    switch (n)
    {
        case 1:
            printf("学号:");
            scanf("%d", &stu.id);
            sprintf(sql, "DELETE FROM %s WHERE id = %d", table_name, stu.id);
            break;
        case 2:
            printf("姓名:");
            scanf("%s", stu.name);
            sprintf(sql, "DELETE FROM %s WHERE name = '%s'", table_name, stu.name);
            break;
        case 3:
            sprintf(sql, "DELETE FROM %s", table_name);
            break;
        default:
            fprintf(stderr, "[ERROR] 无效的选择: %d\n", n);
    }

    ret = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
    if (ret != SQLITE_OK)
    {
        fprintf(stderr, "[ERROR] 删除数据失败: %s\n", errmsg);
        sqlite3_free(errmsg);
        return -1;
    }
    printf("[INFO] 删除数据成功\n");

    return 0;
}
static int do_update(sqlite3 *db, char *table_name)
{
    Student stu;
    int ret      = 0;
    char sql[N]  = {};
    char *errmsg = NULL;
    int n = 0;

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
            sprintf(sql, "UPDATE %s SET score=%d WHERE id=%d", table_name, stu.score, stu.id);
            break;
        case 2:
            printf("姓名:");
            scanf("%s", stu.name);
            printf("请输入修改后的成绩:");
            scanf("%d", &stu.score);
            sprintf(sql, "UPDATE %s SET score=%d WHERE name='%s'", table_name, stu.score, stu.name);
            break;
        default:
            fprintf(stderr, "[ERROR] 无效的选择: %d\n", n);
    }

    ret = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
    if (ret != SQLITE_OK)
    {
        fprintf(stderr, "[ERROR] 更新数据失败: %s\n", errmsg);
        sqlite3_free(errmsg);
        return -1;
    }
    printf("[INFO] 更新数据成功\n");

    return 0;
}

static int do_query(sqlite3 *db, char *table_name)
{
    char *errmsg = NULL;
    char sql[N]  = {};
    int ret      = 0;
    Student stu;
    int n;

    printf("查询方式: 1.查询所有数据 2.查询指定学号的数据 3.查询指定姓名的数据\n");
    printf("请选择:");
    scanf("%d", &n);
    switch (n)
    {
        case 1:
            sprintf(sql, "SELECT * FROM %s;", table_name);
            break;
        case 2:
            printf("学号:");
            scanf("%d", &stu.id);
            sprintf(sql, "SELECT * FROM %s WHERE id=%d;", table_name, stu.id);
            break;
        case 3:
            printf("姓名:");
            scanf("%s", stu.name);
            sprintf(sql, "SELECT * FROM %s WHERE name='%s';", table_name, stu.name);
            break;
        default:
            fprintf(stderr, "[ERROR] 无效的选择: %d\n", n);
    }

    printf("查询结果如下:\n");
    ret = sqlite3_exec(db, sql, select_callback, NULL, &errmsg);
    if (ret != SQLITE_OK)
    {
        fprintf(stderr, "[ERROR] 查询数据失败: %s\n", errmsg);
        sqlite3_free(errmsg);
        return -1;
    }
    printf("[INFO] 查询数据成功\n");

    return 0;
}

static int select_callback(void *data, int argc, char **argv, char **azColName)
{
    int i = 0;

    for (i = 0; i < argc; i++)
    {
        switch (i)
        {
            case 0:
                printf("学号:%s\n", argv[i]);
                break;
            case 1:
                printf("姓名:%s\n", argv[i]);
                break;
            case 2:
                printf("性别:%s\n", argv[i]);
                break;
            case 3:
                printf("成绩:%s\n", argv[i]);
                break;
            default:
                fprintf(stderr, "[ERROR] 未知列:%s:%s\n", azColName[i], argv[i] ? argv[i] : "NULL");
                break;
        }
    }

    printf("\n");

    return 0;
}
