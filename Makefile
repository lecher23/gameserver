CC=g++ -std=c++11 -g 
SUBDIRS=$(shell ls -l | grep ^d | grep -v tools |awk '{if($$9 != "build") print $$9}')
ROOT_DIR=$(shell pwd)
BIN=server
OBJS_DIR=build/obj
BIN_DIR=build/bin
CUR_SOURCE=${wildcard *.cpp}
CUR_OBJS=${patsubst %.cpp, %.o, $(CUR_SOURCE)}
#将以下变量导出到子shell中，本次相当于导出到子目录下的makefile中
export CC BIN OBJS_DIR BIN_DIR ROOT_DIR
#需要先执行SUBDIRS最后才能是DEBUG
all:$(SUBDIRS) $(CUR_OBJS) DEBUG
#递归执行子目录下的makefile文件，这是递归执行的关键
$(SUBDIRS):ECHO
	make -C $@
DEBUG:ECHO
    #直接去debug目录下执行makefile文件
	make -C build
ECHO:
	@echo $(SUBDIRS)
$(CUR_OBJS):%.o:%.cpp
	$(CC) -c $^ -o $(ROOT_DIR)/$(OBJS_DIR)/$@
clean:
	@rm $(OBJS_DIR)/*.o
