#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "MQTTClient.h"

// EMQX
#define ADDRESS  "tcp://192.168.173.179:1883"
#define CLIENTID "mqtt_test"
#define TOPIC    "test/taishan"
#define PAYLOAD  "{\"test\": \"hello\", \"device\": \"rk3566\"}"
#define QOS      1
#define TIMEOUT  10000L

int main(int argc, char *argv[])
{
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    MQTTClient_message pubmsg           = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;
    int rc;

    // 1. 创建客户端
    printf("Creating client...\n");
    // 第三个参数 persistence_context = NULL 表示不使用本地文件缓存消息
    if ((rc = MQTTClient_create(&client, ADDRESS, CLIENTID,
                                MQTTCLIENT_PERSISTENCE_NONE, NULL)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to create client, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }

    // 2. 配置连接选项
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession      = 1; // 1=不保留会话，0=保留
    // conn_opts.username = "user"; // 如果Broker需要密码
    // conn_opts.password = "pass";

    // 3. 连接 Broker
    printf("Connecting to broker: %s ...\n", ADDRESS);
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        MQTTClient_destroy(&client); // 记得清理
        exit(EXIT_FAILURE);
    }
    printf("Connected!\n");

    // 4. 准备并发布消息
    printf("Publishing to topic: %s\n", TOPIC);
    pubmsg.payload    = PAYLOAD;
    pubmsg.payloadlen = (int)strlen(PAYLOAD);
    pubmsg.qos        = QOS;
    pubmsg.retained   = 0;

    // 异步发布，获取 token
    MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);

    // 等待发布完成 (阻塞直到收到 PUBACK)
    printf("Waiting for delivery token %d...\n", token);
    rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);

    printf("Message with delivery token %d delivered\n", token);

    // 5. 断开连接
    // 等待 10秒让未发完的消息发完
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);

    printf("Done.\n");
    return rc;
}