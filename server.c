#include <stdio.h>
#include <stdlib.h>

#include "server_utils.h"

int main() {
    int my_socket = start(6969);
    while (1) {
        int connection = connect_client(my_socket);
        if (connection) {
            exit(EXIT_FAILURE);
        }
    }
}
