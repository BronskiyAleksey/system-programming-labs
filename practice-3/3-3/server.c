#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8080
#define NUM_COUNT 20
#define BUFFER_SIZE (NUM_COUNT * sizeof(int))

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    int numbers[NUM_COUNT];
    long long sum = 0;

    // Создание сокета
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Установка опции SO_REUSEADDR
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Настройка адреса сервера
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Привязка сокета к адресу
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Прослушивание входящих соединений
    if (listen(server_fd, 3) < 0) {
        perror("listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Сервер запущен на порту %d. Ожидание подключений...\n", PORT);

    // Принятие соединения
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Клиент подключился.\n");

    // Получение данных от клиента
    int bytes_received = recv(new_socket, numbers, BUFFER_SIZE, 0);
    if (bytes_received < 0) {
        perror("recv failed");
        close(new_socket);
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (bytes_received != BUFFER_SIZE) {
        fprintf(stderr, "Получено неверное количество данных\n");
        close(new_socket);
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Вычисление суммы
    for (int i = 0; i < NUM_COUNT; i++) {
        sum += numbers[i];
    }

    printf("Получены числа: ");
    for (int i = 0; i < NUM_COUNT; i++) {
        printf("%d ", numbers[i]);
    }
    printf("\nСумма: %lld\n", sum);

    // Отправка результата клиенту
    if (send(new_socket, &sum, sizeof(sum), 0) < 0) {
        perror("send failed");
        close(new_socket);
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Результат отправлен клиенту.\n");

    // Закрытие соединений
    close(new_socket);
    close(server_fd);
    printf("Сервер завершён.\n");

    return 0;
}
