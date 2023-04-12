compile:
	g++  -Wall -g3 -fsanitize=address -pthread -std=c++14 -lprotobuf server.cpp project.pb.cc -o server
	g++  -Wall -g3 -fsanitize=address -pthread -std=c++14 -lprotobuf client.cpp project.pb.cc -o client
FLAGS    = -L /lib64
LIBS     = -lusb-1.0 -l pthread

