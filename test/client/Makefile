###### Record 进程 ########
# 系统平台
ARCH = $(shell arch)
BITS =
ifeq "$(findstring x86_64,$(ARCH))" "x86_64"
BITS = 64
endif
#开关
ON = 1
OFF = 0

#版本/
VERSION =


#日期
#DATE=$(shell date +%Y%m%d%H%M%S)

# 目标文件名字
#APP = control_$(VERSION)_$(ARCH)
APP = client
BIN_DIR = bin
$(shell mkdir -p $(BIN_DIR)) 
TARGET =$(BIN_DIR)/$(APP)


# c源文件
SRC = $(wildcard ./*.c)\
 $(wildcard ./../../osa/src/*.c)\
 $(wildcard ./../../communication/*.c)\
 $(wildcard ./../../socket/*.c)
# object文件i
OBJS = $(SRC:.c=.o)
OBJS_DIR = objs
$(shell mkdir -p $(OBJS_DIR)) 
#OBJS_DEST = $(wildcard $(OBJS_DIR)/*.o)
EXTERN_OBJ :=  $(foreach n,$(notdir $(OBJS)),$(OBJS_DIR)/$(n))


# 编译器
CC = gcc
STRIP = strip

# 头文件路径，可以任意指定头文件
INCLUDE := -I./\
 -I./../../osa/inc\
 -I./../../communication/\
 -I./../../socket/


# 链接库
LIBS := -lpthread -lrt -lm -ldl

#LOCAL_LIBS += -llzma
#LOCAL_LIBS += -lxml2
#LOCAL_LIBS += -lva
LIBS += '-Wl,--start-group' $(LOCAL_LIBS) '-Wl,--end-group'

# 编译选项
CFLAGS = -g -Wall -D_FILE_OFFSET_BITS=64 
#-D _MEETING
#-Werror
#CFLAGS := -O2 -Wall -Wno-strict-aliasing -g
#CFLAGS = -O2
all: $(TARGET)

# 定义目标文件生成规则
$(TARGET):FIRST $(OBJS)
	echo #$(OBJS)#
	echo #$(EXTERN_OBJ)#
	$(CC) -o $(TARGET) $(EXTERN_OBJ) $(LIBS)

FIRST:
	rm -rf $(OBJS)
	
.SUFFIXES:.c	
# 宏定义
# 定义obejct文件生成规则
.c.o:
	$(CC) $(INCLUDE) $(CFLAGS) -c $<
	@mv *.o $(OBJS_DIR)
# 打安装包

# 定义清理函数
.PHONY:clean
install:
#	$(STRIP) ./bin/record
#	cp ./bin/record  /home/ydl/workdir/rfs/opt/dvr_rdk/ti816x_2.8/bin
clean:
	@rm -rf $(OBJS_DIR) $(TARGET)
#	@rm -rf pkg/$(APP)*
	@rm -rf $(OBJS_DIR)
	@rm -rf $(BIN_DIR)
#	@rm -rf version.h
#	@rm -rf control_config.xml log_file
