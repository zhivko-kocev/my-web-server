#ifndef SERVER_UTILS_H
#define SERVER_UTILS_H

int start(int port);

int connect_client(int sock);

void *worker(void *client);

void response(const char* filename, const char* extension,char* content, int* content_length);

//void log(const char *message);

#define MAX_CONNECTIONS 5
#define BUFFER_SIZE 4096

#endif // SERVER_UTILS_H
