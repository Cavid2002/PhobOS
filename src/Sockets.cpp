#include <stdlib.h>
#include <string.h>


#include "Sockets.hpp"

#pragma comment(lib, "Ws2_32.lib")

void error(const char* msg)
{
    fprintf(stderr, "[ERROR]%s\n", msg);
    exit(EXIT_FAILURE);
}

SOCKET create_sock()
{
    SOCKET l_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(l_sock == INVALID_SOCKET)
    {
        error("SOCKET!");
    }
    return l_sock;
}

void bind_to_port(SOCKET sock, uint16_t port)
{
    SOCKADDR_IN name;
    name.sin_addr.S_un.S_addr = INADDR_ANY;
    name.sin_family = AF_INET;
    name.sin_port = htons(port);
    char opt = 1;

    if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == SOCKET_ERROR)
    {
        error("REUSE!");
    }

    if(bind(sock, (sockaddr*)&name, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
    {
        error("BIND!");
    }
    return;
}

void start_listening(SOCKET sock, uint16_t q_size)
{
    if(listen(sock, q_size) == SOCKET_ERROR)
    {
        error("LISTEN!");
    }
    return;
}

SOCKET accept_connection(SOCKET sock, sockaddr_in* client)
{
    int client_size = sizeof(sockaddr_in);
    int cln_sock = accept(sock, (sockaddr*)client, &client_size);
    if(cln_sock == SOCKET_ERROR)
    {
        error("ACCEPT");
    }

    return cln_sock;
}

int connect_to_addr(SOCKET sock, const char* ip_addr, uint16_t port)
{
    sockaddr_in name;
    name.sin_addr.S_un.S_addr = inet_addr(ip_addr);
    name.sin_port = htons(port);
    name.sin_family = AF_INET;
    int ret = connect(sock, (sockaddr*)&name, sizeof(name));

    return ret;
}


char** split_str(char* str, char c)
{
    char* p1 = str;
    char* p2 = str;
    int token_count = 0;
    while(p1 != NULL)
    {
        p1 = strchr(p1 + 1, c);
        token_count++;
    } 
    char** tokens = (char**)malloc(sizeof(char*) * (token_count + 1));
    if(token_count == 1)
    {
        tokens[0] = (char*)malloc(strlen(str) + 1);
        strcpy(tokens[0], str);
        tokens[1] = NULL;
        return tokens;
    }

    for(int i = 0; true; i++)
    {
        p1 = strchr(p2, c);
        if(p1 == NULL)
        {
            tokens[i] = (char*)malloc(strlen(p2) + 1);
            strcpy(tokens[i], p2);
            break;
        }
        *p1 = '\0';
        tokens[i] = (char*)malloc(strlen(p2) + 1);
        strcpy(tokens[i], p2);
        *p1 = c;
        p2 = p1 + 1;

    }
    tokens[token_count] = NULL;
    return tokens;
}


void free_tokens(char** tokens)
{
    for(int i = 0; tokens[i] != NULL; i++)
    {
        free(tokens[i]);
    }
    free(tokens);
}


uint32_t get_file_size(FILE* file)
{
    fseek(file, 0, SEEK_END);
    uint32_t res = ftell(file);
    fseek(file, 0, SEEK_SET);
    return res;
}

int execute_proc(const char* command, char* buffer, SOCKET sock)
{
    char cmd[50] = "cmd.exe /C "; // Command to execute
    strcat(cmd, command);
    // Variables for the pipe handles
    HANDLE hRead, hWrite;
    SECURITY_ATTRIBUTES saAttr;

    // Set the security attributes for the pipe handles
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    // Create the pipe
    if (!CreatePipe(&hRead, &hWrite, &saAttr, 0)) {
        fprintf(stderr, "CreatePipe failed: %d\n", GetLastError());
        return 1;
    }

    // Ensure the read handle to the pipe is not inherited
    if (!SetHandleInformation(hRead, HANDLE_FLAG_INHERIT, 0)) {
        fprintf(stderr, "SetHandleInformation failed: %d\n", GetLastError());
        return 1;
    }

    // Setup the process info and startup info structs
    PROCESS_INFORMATION piProcInfo;
    STARTUPINFO siStartInfo;
    ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
    ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
    siStartInfo.cb = sizeof(STARTUPINFO);
    siStartInfo.hStdError = hWrite;
    siStartInfo.hStdOutput = hWrite;
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

    // Create the child process
    if (!CreateProcess(NULL, (LPSTR)cmd, NULL, NULL, TRUE, 0, NULL, NULL, &siStartInfo, &piProcInfo)) {
        fprintf(stderr, "CreateProcess failed: %d\n", GetLastError());
        return 1;
    }
    // Close the write end of the pipe in the parent process
    CloseHandle(hWrite);

    // Read the output from the child process
    int bufferSize = BUFF_SIZE;
    char* temp = buffer;
    DWORD bytesRead;
    while(true)
    {
        if(bufferSize == 0)
        { 
            send(sock, buffer, BUFF_SIZE, 0);
            bufferSize = BUFF_SIZE;
            temp = buffer;
        }
        ReadFile(hRead, temp, bufferSize, &bytesRead, NULL);
        if(bytesRead == 0)
        {
            *temp = '\n';
            send(sock, buffer, BUFF_SIZE - bufferSize + 1, 0);
            break;
        }
        temp += bytesRead;
        bufferSize -= bytesRead;
    }

    // Close the read end of the pipe
    CloseHandle(hRead);

    // Wait for the child process to finish
    WaitForSingleObject(piProcInfo.hProcess, INFINITE);

    // Close process and thread handles
    CloseHandle(piProcInfo.hProcess);
    CloseHandle(piProcInfo.hThread);

    return BUFF_SIZE - bufferSize;

}


void test_output(char* buff)
{
    char** tokens = split_str(buff, ' ');
    printf("Command:%s\n", buff);
    for(int i = 0; true; i++)
    {
        if(tokens[i] == NULL) break; 
        printf("Tokens: %s\n", tokens[i]);
    }

    free_tokens(tokens);
}
