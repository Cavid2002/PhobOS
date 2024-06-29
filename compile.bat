cd src
g++ -c Client.cpp -o Client.o
g++ -c Server.cpp -o Server.o
g++ -c Sockets.cpp -o Sockets.o
ar -rcs libNET.a *.o
del *.o
cd ..

g++ server_main.cpp -o .\bin\server_main -L.\src\ -l:libNET.a -lwsock32
g++ client_main.cpp -o .\bin\client_main -L.\src\ -l:libNET.a -lwsock32
