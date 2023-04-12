compile:
	g++ -Wall -g3 -fsanitize=address -pthread -std=c++11 server.cpp -o server
	g++ -Wall -g3 -fsanitize=address -pthread -std=c++11 client.cpp -o client
FLAGS    = -L /lib64
LIBS     = -lusb-1.0 -l pthread

