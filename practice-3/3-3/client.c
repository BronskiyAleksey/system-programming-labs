#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080
#define NUM_COUNT 20
#define SERVER_IP "127.0.0.1"

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    int numbers[NUM_COUNT];
    long long result;

    // Заполнение массива числами (можно заменить на ввод с клавиатуры)
    printf("Введите %d чисел:\n", NUM_COUNT);
    for (int i = 0; i < NUM_COUNT; i++) {
        printf("Число %d: ", i + 1);
        scanf("%d", &numbers[i]);
    }

    // Создание сокета
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket creation error");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Преобразование IP-адреса
    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
        perror("invalid address / address not supported");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Подключение к серверу
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connection failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    printf("Подключено к серверу %s:%d\n", SERVER_IP, PORT);

    // Отправка чисел серверу
    if (send(sock, numbers, NUM_COUNT * sizeof(int), 0) < 0) {
        perror("send failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    printf("Числа отправлены серверу.\n");

    // Получение результата от сервера
    if (recv(sock, &result, sizeof(result), 0) < 0) {
        perror("recv failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    printf("Получен результат от сервера: %lld\n", result);

    // Закрытие соединения
    close(sock);
    printf("Соединение закрыто.\n");

    return 0;
}
