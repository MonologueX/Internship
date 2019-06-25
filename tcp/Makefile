all:server client

server:tcp_server.cpp
	g++ -std=c++11 tcp_server.cpp -pthread -o server 

client:
	g++ -std=c++11 tcp_client.cpp -o client

.PHONY:clean
clean:
	rm -f server client
