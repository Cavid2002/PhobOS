#include "src/Client.hpp"

int main()
{
    WSADATA wsa;
	if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
	{
		return 1;
	}

    
    Client client("127.0.0.1", 4000);
    client.establish_connection();
}

