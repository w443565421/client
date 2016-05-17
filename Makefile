#server : demo.o server.o
#	g++ demo.o server.o -o server

client: client.o
	g++ client.o -o client

client.o : client.cpp
	g++ -c client.cpp

#server.o : server.cpp
#	g++ -c server.cpp

#demo.o : demo.cpp
#	g++ -c demo.cpp


clean:
	rm -f *.o
	rm -f *~
