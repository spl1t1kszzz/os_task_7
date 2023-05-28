#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

int main() {
    int sockfd;
    // представляем адрес сервера
    struct sockaddr_un server_addr;
    // создаём новый сокет (возвращает дескриптор)
    if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("Error in socket()!!!\n");
        return 1;
    }
    // чистим структуру
    memset(&server_addr, 0, sizeof(server_addr));
    // домен UNIX
    server_addr.sun_family = AF_UNIX;
    // файл сокета
    strncpy(server_addr.sun_path, "./dsock_file", sizeof(server_addr.sun_path) - 1);

    // коннектимся с удалённым сервером
    if (connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr))) {
        perror("Error in connect()!!!\n");
        return 1;
    }


    char input[128];
    ssize_t ret;

    while (1) {
        memset(input, 0, sizeof(input));
        fgets(input, sizeof(input), stdin);

        ret = write(sockfd, input, strlen(input));
        if (ret == -1 || ret == 0) {
            perror("Error in write()!!!\n");
            break;
        }

        ret = read(sockfd, input, sizeof(input));
        if (ret == -1 || ret == 0) {
            perror("Error in read()!!!\n");
            break;
        }

        printf("Server response: %s\n", input);
    }

    close(sockfd);

    return 0;
}
