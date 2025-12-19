# ================= 配置区域 =================
REMOTE_HOST = root@192.168.7.2
# 目标机上的部署路径
TARGET_PATH = $(REMOTE_HOST):/root/mqtt_test

# Paho 库在 WSL 宿主机上的绝对路径
# 注意：根据之前的 make install 结构，头文件通常在 usr/local/include，库在 usr/local/lib
PAHO_HOME = /home/ubuntu/arm-libs/paho_mqtt_c/usr/local

INC_DIR = ./inc
SRC_DIR = ./src
BUILD_DIR = ./build
TARGET = $(BUILD_DIR)/main

CROSS_COMPILE ?= aarch64-linux-gnu-
CC = $(CROSS_COMPILE)gcc

# CFLAGS: 增加 Paho 的头文件路径
CFLAGS = -Wall -I$(INC_DIR) -I$(PAHO_HOME)/include

# LDLIBS: 增加 Paho 的库路径和链接选项
# -L: 指定库文件在哪里
# -lpaho-mqtt3cs: 链接同步+SSL版本的库 (推荐默认用这个)
# -lssl -lcrypto: Paho 依赖 OpenSSL
LDLIBS = -L$(PAHO_HOME)/lib -lpaho-mqtt3cs -lssl -lcrypto -lpthread

# ================= 编译规则 =================
# ./src/*.c
SRCS = $(shell find $(SRC_DIR) -name '*.c')
# ./src/*.c => ./build/*.o
OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))
# ./src/*.c => ./build/*.d
DEPS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.d, $(SRCS))

# 默认目标
all: $(TARGET)

DEPFLAGS = -MMD -MP -MF $(@:.o=.d) -MT $@

# 编译
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(DEPFLAGS) -c -o $@ $<

# 链接
$(TARGET): $(OBJS)
	@echo "Linking $@..."
	@mkdir -p $(dir $@)
	$(CC) -o $(TARGET) $(OBJS) $(LDLIBS)

# 部署 (包含可执行文件 + 动态库)
# 注意：这里我加了一行 scp 库文件的命令，方便你调试，避免板子上缺库
push: $(TARGET)
	@echo "Pushing to $(REMOTE_HOST)..."
	@# ssh $(REMOTE_HOST) "mkdir -p /root/mqtt_test"
	scp $(TARGET) $(TARGET_PATH)
	@# 可选：如果板子上还没库，把库也传过去
	@# scp $(PAHO_HOME)/lib/libpaho-mqtt3cs.so* $(TARGET_PATH)/

clean:
	@echo "Cleaning..."
	rm -rf $(BUILD_DIR)

-include $(DEPS)

.PHONY: all clean push