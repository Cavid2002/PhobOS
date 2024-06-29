#include "Sockets.hpp"

#define MAX_QUEUE 10

class Server{
private:
    uint16_t port;
    SOCKET server_sock;
    uint16_t q_size;
    uint16_t client_num;
    SOCKET client_arr[MAX_QUEUE];
    char buff_send[BUFF_SIZE];
    char buff_recv[BUFF_SIZE];
    void initialize_server();
    void control();

public:
    Server(uint16_t port, uint16_t q_size);
    void upload_file(const char* filename, SOCKET client_sock);
    void download_file(const char* filename, SOCKET client_sock);
    void get_output(SOCKET sock);
    void run();
    void handle_client(SOCKET sock);
};