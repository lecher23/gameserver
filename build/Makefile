OBJS=*.o
ODIR=obj
$(ROOT_DIR)/$(BIN_DIR)/$(BIN):$(ODIR)/$(OBJS)
	$(CC) -g -o $@ $^ -l pthread -L ${EXTRA_LIB_DIR} -l lua5.1 -l mysqlclient -l glog -l glfw
