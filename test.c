//
// Created by zhivko-kocev on 9/17/24.
//

#include <stdio.h>
#include <unistd.h>

int main() {
    FILE *file = fopen("../static/index.html", "r");
    char cwd[100];
    getcwd(cwd, sizeof(cwd));
    printf("%s\n", cwd);
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    printf("File opened successfully!\n");
    fclose(file);
    return 0;
}
