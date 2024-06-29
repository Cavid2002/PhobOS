#include "src/Server.hpp"

int main()
{
    WSADATA wsa;
	
	if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
	{
		return 1;
	}
    Server server(4000, 4);
    server.run();
}