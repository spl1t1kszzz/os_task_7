#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

int main() {
    int sockfd;
    int clt_sock;
    // хранит в себе семейство адресов, путь к файлу сокета
    struct sockaddr_un server_addr, client_addr;
    // unsigned int
    socklen_t client_addr_len;


    // AF_UNIX - между процессами на одном компе
    // SOCK_STREAM - потоковая передача данных (TCP протокол)
    // 0 - выбор протокола по умолчанию
    if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket() failed\n");
        exit(EXIT_FAILURE);
    }
    // обнуляем структуру
    memset(&server_addr, 0, sizeof(server_addr));
    // используем локальный домен
    server_addr.sun_family = AF_UNIX;
    // копируем "socket_path" в sun_path ??
    strncpy(server_addr.sun_path, "socket_path", sizeof(server_addr.sun_path) - 1);
    // копируем "./dsock_file" в sun_path ??
    strcpy(server_addr.sun_path, "./dsock_file");
    // привязываем сокет 'sockfd' к адресу, указанному в структуре (server_addr)

    if (bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
        perror("Error in bind() !!!1\n");
        return 1;
    }
    // устанавливаем sockfd в режим прослушивания
    // 5 - максимальное количество ожидающих соединений, которые могут находиться
    // в очереди ожидания, ожидающих обработки сервером
    if (listen(sockfd, 5) == -1) {
        perror("Error in listen() !!!1\n");
        return 1;
    }


    while (1) {
        // храним инфу о клиенте
        client_addr_len = sizeof(client_addr);
        // чистим клиента
        memset(&client_addr, 0, sizeof(struct sockaddr_un));
        // принимаем входящее соединения от клиента
        // в client_addr сохраняем инфу об адресе клиента
        // &client_addr_len - указатель на переменную, которая содержит размер структуры
        clt_sock = accept(sockfd, (struct sockaddr *) &client_addr, &client_addr_len);
        if (clt_sock == -1) {
            perror("Error in accept() !!!1\n");
            close(sockfd);
            return 1;
        }

        int pid = fork();
        // пусть теперь ребёнок батрачит
        if (pid == -1) {
            perror("Error in fork() !!!1\n");
            close(clt_sock);
            close(sockfd);
            return 1;
        } else if (pid == 0) {
            break;
        }

        close(clt_sock);
    }

    close(sockfd);

    while (1) {
        int ret;
        char buff[128];

        // читаем что пришло
        ret = read(clt_sock, buff, sizeof(buff));
        if (ret == -1 || ret == 0) {
            printf("stopping\n");
            break;
        }
        printf("%s\n", buff);

        // пишем в сокет мой буффер
        ret = write(clt_sock, buff, ret);
        if (ret == -1 || ret == 0) {
            printf("stopping\n");
            break;
        }
    }

    close(clt_sock);
    // удаляем запись, связанную с адресом сокета
    unlink(server_addr.sun_path);

    return 0;
}