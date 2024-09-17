//
// Created by zhivko-kocev on 9/16/24.
//
#include "server_utils.h"
#include "string_utils.h"

#include <locale.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <regex.h>
#include <string.h>
#include <unistd.h>


int start(int port) {
    int my_socket;
    struct sockaddr_in address;

    if ((my_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Couldn't create the socket!");
        exit(EXIT_FAILURE);
    }

    printf("Socket created successfully!\n");

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(my_socket, (struct sockaddr *) &address, sizeof(address)) == -1) {
        perror("Couldn't bind the socket!");
        close(my_socket);
        exit(EXIT_FAILURE);
    }

    printf("Socket binded successfully!\n");

    if (listen(my_socket, 5) == -1) {
        perror("Couldn't listen on socket!");
        close(my_socket);
        exit(EXIT_FAILURE);
    }

    printf("Socket listening successfully on http://localhost:%d!\n", port);
    return my_socket;
}

int connect_client(int server_socket) {
    struct sockaddr_in client;
    socklen_t len = sizeof(client);
    int client_socket;

    if ((client_socket = accept(server_socket, (struct sockaddr *) &client, &len)) == -1) {
        perror("Couldn't accept the client!");
        return -1;
    }

    printf("Socket accepted successfully!\n");

    pthread_t thread;

    if (pthread_create(&thread, NULL, worker, (void *) (intptr_t) client_socket)) {
        perror("Couldn't create thread!");
        close(client_socket);
        return -1;
    }

    pthread_detach(thread);
    return 0;
}

void *worker(void *client) {
    int client_socket = (intptr_t) client;

    char *buffer = malloc(BUFFER_SIZE * sizeof(char));
    if (!buffer) {
        perror("Couldn't allocate memory for buffer");
        close(client_socket);
        pthread_exit(NULL);
    }

    size_t bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);

    if (bytes_received == -1) {
        perror("Couldn't receive data from client!");
        free(buffer);
        close(client_socket);
        pthread_exit(NULL);
    }

    buffer[bytes_received] = '\0';

    regex_t regex;
    regcomp(&regex, "^GET /([^ ]*) HTTP/1.1", REG_EXTENDED);
    regmatch_t match[2];

    if (!regexec(&regex, buffer, 2, match, 0)) {
        char path[BUFFER_SIZE];
        int length = match[1].rm_eo - match[1].rm_so;
        strncpy(path, buffer + match[1].rm_so, length);
        path[length] = '\0';

        char *file_name = decode(path);
        char *file_extension = get_extension(file_name);

        char *res = malloc(BUFFER_SIZE * sizeof(char));
        int res_length = 0;
        response(file_name, file_extension, res, &res_length);

        send(client_socket, res, res_length, 0);

        free(res);
        free(file_name);
    }

    regfree(&regex);
    free(buffer);
    close(client_socket);
    return NULL;
}

void response(const char *filename, const char *extension, char *content, int *content_length) {
    // Define HTTP status line and headers
    const char *status_line = "HTTP/1.1 200 OK\r\n";
    const char *connection_header = "Connection: close\r\n";
    char *content_type_header = get_mime(extension);
    char content_length_header[256];
    char file_buffer[BUFFER_SIZE];
    char file_path[BUFFER_SIZE];

    snprintf(file_path, sizeof(file_path), "../static/%s", filename);

    if (!strcmp(filename, "")) {
        snprintf(file_path, sizeof(file_path), "../static/index.html");
    }

    printf("Trying to open file: %s\n", file_path);

    FILE *file = fopen(file_path, "rb");
    if (!file) {
        perror("Couldn't open file");
        snprintf(content, BUFFER_SIZE,
                 "HTTP/1.1 404 Not Found\r\nConnection: close\r\nContent-Type: text/plain\r\nContent-Length: 15\r\n\r\nFile not found");
        *content_length = strlen(content);
        return;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (file_size >= BUFFER_SIZE) {
        fprintf(stderr, "File size is too large\n");
        fclose(file);
        snprintf(content, BUFFER_SIZE,
                 "HTTP/1.1 500 Internal Server Error\r\nConnection: close\r\nContent-Type: text/plain\r\nContent-Length: 21\r\n\r\nFile too large to handle");
        *content_length = strlen(content);
        return;
    }

    fread(file_buffer, 1, file_size, file);
    fclose(file);

    snprintf(content_length_header, sizeof(content_length_header), "Content-Length: %ld\r\n", file_size);

    snprintf(content, BUFFER_SIZE, "%s%s%s%s\r\n", status_line, content_type_header, content_length_header,
             connection_header);
    strncat(content, file_buffer, file_size);

    *content_length = strlen(content);
}
