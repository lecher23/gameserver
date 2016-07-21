OBJS = cgserver.o addrspec.o tcpserver.o httppacketparser.o httppacket.o handler.o httpresponsepacket.o packethandler.o
CC = g++ -g 
server:$(OBJS)
	g++ -o server $(OBJS)
cgserver.o:cgserver.cpp addrspec.o tcpserver.o
addrspec.o:addrspec.cpp
tcpserver.o:tcpserver.cpp
httppacketparser.o: httppacketparser.cpp
httppacket.o:httppacket.cpp
handler.o:handler.cpp
packethandler.o:packethandler.cpp
httpresponsepacket.o:httpresponsepacket.cpp
.PHONY : clean
clean:
	-rm *.o
