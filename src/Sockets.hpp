#include <winsock.h>
#include <stdint.h>
#include <stdio.h>

#define BUFF_SIZE 4096


SOCKET create_sock();

void bind_to_port(SOCKET sock, uint16_t port);

void start_listening(SOCKET sock, uint16_t q_size);

SOCKET accept_connection(SOCKET sock, sockaddr_in* client);

int connect_to_addr(SOCKET sock, const char* ip_addr, uint16_t port);

char** split_str(char* str, char c);

void free_tokens(char** tokens);

uint32_t get_file_size(FILE* file);

int execute_proc(const char* command, char* buffer, SOCKET sock);

void test_output(char* buff);
