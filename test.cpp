#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <string>
#include <direct.h>


char** split_str(char* str, char c)
{
    char* p1 = str;
    char* p2 = str;
    int token_count;
    while(p1 != NULL)
    {
        p1 = strchr(p1 + 1, c);
        token_count++;
    } 
    char** tokens = (char**)malloc(sizeof(char*) * (token_count + 1));

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

void free_token(char** tokens)
{
    for(int i = 0; tokens[i] != NULL; i++)
    {
        free(tokens[i]);
    }
    free(tokens);
}



int execute_proc(const char* command)
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
    const int bufferSize = 4096;
    char buffer[bufferSize];
    DWORD bytesRead;
    while (ReadFile(hRead, buffer, bufferSize, &bytesRead, NULL) && bytesRead != 0) {
        fwrite(buffer, 1, bytesRead, stdout);
    }

    // Close the read end of the pipe
    CloseHandle(hRead);

    // Wait for the child process to finish
    WaitForSingleObject(piProcInfo.hProcess, INFINITE);

    // Close process and thread handles
    CloseHandle(piProcInfo.hProcess);
    CloseHandle(piProcInfo.hThread);

    return 0;

}

 
int main()
{
    char buff[10];
    fgets(buff, 10, stdin);
    printf("%d\n", strlen(buff));
    return 0;
}