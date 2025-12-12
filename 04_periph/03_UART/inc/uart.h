#ifndef _UART_H
#define _UART_H

/**
 * @brief 初始化串口
 * @param port 串口设备号，如 3
 * @param baudrate 波特率，如 B115200
 * @return int 成功返回文件描述符fd，失败返回 -1
 */
int uart_init(int port, int baudrate);

/**
 * @brief 释放串口
 * @param fd 串口文件描述符
 * @return void
 */
void uart_deinit(int fd);



/**
 * @brief 发送数据
 * @param fd 串口文件描述符
 * @param data 要发送的数据指针
 * @param len 要发送的数据长度
 * @return int 成功返回发送的字节数，失败返回 -1
 */
int uart_send(int fd, char *data, int len);



/**
 * @brief 接收数据
 * @param fd 串口文件描述符
 * @param buf 接收数据的缓冲区指针
 * @param len 要接收的数据长度
 * @return int 成功返回接收到的字节数，返回 0 表示没接收到数据，返回 -1 表示接收失败
 */
int uart_receive(int fd, char *buf, int len);

#endif
