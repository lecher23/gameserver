OBJS = cgserver.o addrspec.o tcpserver.o httppacketparser.o httppacket.o  httpresponsepacket.o packethandler.o socketprocessor.o 
CC = g++ -g
CCMD = g++ -g -c
server:$(OBJS)
	g++ -g -o server $(OBJS) -l pthread
cgserver.o:cgserver.cpp
	$(CCMD) -o $@ $<
addrspec.o:addrspec.cpp
	$(CCMD) -o $@ $<
tcpserver.o:tcpserver.cpp
	$(CCMD) -o $@ $<
httppacketparser.o: httppacketparser.cpp
	$(CCMD) -o $@ $<
httppacket.o:httppacket.cpp
	$(CCMD) -o $@ $<
packethandler.o:packethandler.cpp
	$(CCMD) -o $@ $<
socketprocessor.o:socketprocessor.cpp
	$(CCMD) -o $@ $<
httpresponsepacket.o:httpresponsepacket.cpp
	$(CCMD) -o $@ $<
.PHONY : clean
clean:
	-rm *.o
