#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#define FIFO_NAME "my_fifo"
#define BUFFER_SIZE 1024
#define OUTPUT_FILE "copy.txt"

// Функция проверки существования файла
int file_exists(const char *filename) {
    struct stat buffer;
    return (stat(filename, &buffer) == 0);
}

int main() {
    int fifo_fd, input_fd, output_fd;
    char file_path[BUFFER_SIZE];
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    // Шаг 1: Создание FIFO
    if (mkfifo(FIFO_NAME, 0666) == -1) {
        if (errno != EEXIST) {
            perror("Ошибка создания FIFO");
            exit(EXIT_FAILURE);
        }
    }

    printf("FIFO '%s' создан (или уже существует)\n", FIFO_NAME);

    // Шаг 2: Открытие FIFO для чтения
    fifo_fd = open(FIFO_NAME, O_RDONLY);
    if (fifo_fd == -1) {
        perror("Ошибка открытия FIFO для чтения");
        unlink(FIFO_NAME);
        exit(EXIT_FAILURE);
    }

    // Шаг 3: Чтение пути к файлу из FIFO
    bytes_read = read(fifo_fd, file_path, sizeof(file_path) - 1);
    if (bytes_read == -1) {
        perror("Ошибка чтения из FIFO");
        close(fifo_fd);
        unlink(FIFO_NAME);
        exit(EXIT_FAILURE);
    }
    file_path[bytes_read] = '\0';

    // Удаляем возможные пробелы и переносы в конце пути
    while (bytes_read > 0 && (file_path[bytes_read - 1] == '\n' ||
           file_path[bytes_read - 1] == ' ' || file_path[bytes_read - 1] == '\r')) {
        file_path[--bytes_read] = '\0';
    }

    printf("Получен путь к файлу: '%s'\n", file_path);

    close(fifo_fd);

    // Проверка существования файла
    if (!file_exists(file_path)) {
        fprintf(stderr, "Ошибка: файл '%s' не существует\n", file_path);
        unlink(FIFO_NAME);
        exit(EXIT_FAILURE);
    }

    // Шаг 4: Открытие исходного файла для чтения
    input_fd = open(file_path, O_RDONLY);
    if (input_fd == -1) {
        perror("Ошибка открытия исходного файла");
        unlink(FIFO_NAME);
        exit(EXIT_FAILURE);
    }

    // Шаг 5: Создание/открытие выходного файла
    output_fd = open(OUTPUT_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (output_fd == -1) {
        perror("Ошибка создания выходного файла");
        close(input_fd);
        unlink(FIFO_NAME);
        exit(EXIT_FAILURE);
    }

    // Шаг 6: Копирование содержимого файла
    while ((bytes_read = read(input_fd, buffer, sizeof(buffer))) > 0) {
        if (write(output_fd, buffer, bytes_read) == -1) {
            perror("Ошибка записи в выходной файл");
            close(input_fd);
            close(output_fd);
            unlink(FIFO_NAME);
            exit(EXIT_FAILURE);
        }
    }

    if (bytes_read == -1) {
        perror("Ошибка чтения исходного файла");
        close(input_fd);
        close(output_fd);
        unlink(FIFO_NAME);
        exit(EXIT_FAILURE);
    }

    // Шаг 7: Закрытие всех дескрипторов
    close(input_fd);
    close(output_fd);

    // Шаг 8: Удаление FIFO
    if (unlink(FIFO_NAME) == -1) {
        perror("Ошибка удаления FIFO");
        exit(EXIT_FAILURE);
    }

    printf("Содержимое файла '%s' успешно скопировано в '%s'\n",
           file_path, OUTPUT_FILE);

    return EXIT_SUCCESS;
}
