server:cgserver.o
	g++ -o server cgserver.o addrspec.o tcpserver.o
cgserver.o:cgserver.cpp addrspec.o tcpserver.o
addrspec.o:addrspec.cpp
tcpserver.o:tcpserver.cpp
