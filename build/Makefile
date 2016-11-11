OBJS=*.o
ODIR=obj
$(BIN_DIR)/$(BIN):$(ODIR)/$(OBJS)
	$(CC) -o $@ $^ -L ${EXTRA_LIB_DIR} -Wl,-Bdynamic -llua5.1 -lmysqlclient -lglog -lhiredis -lpthread -Wl,-static -lcppnetlib-server-parsers -static-libstdc++ -static-libgcc
