#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    int pipe_to_child[2];  // Канал: родитель → дочерний
    int pipe_to_parent[2]; // Канал: дочерний → родитель
    pid_t pid;
    int number, square;

    // Создаём первый канал (родитель → дочерний)
    if (pipe(pipe_to_child) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Создаём второй канал (дочерний → родитель)
    if (pipe(pipe_to_parent) == -1) {
        perror("pipe");
        close(pipe_to_child[0]);
        close(pipe_to_child[1]);
        exit(EXIT_FAILURE);
    }

    // Создаём дочерний процесс
    pid = fork();
    if (pid == -1) {
        perror("fork");
        close(pipe_to_child[0]);
        close(pipe_to_child[1]);
        close(pipe_to_parent[0]);
        close(pipe_to_parent[1]);
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Код дочернего процесса

        // Закрываем неиспользуемые концы каналов
        close(pipe_to_child[1]); // Закрываем запись в канал родитель→дочерний
        close(pipe_to_parent[0]); // Закрываем чтение из канала дочерний→родитель

        // Читаем число от родителя
        ssize_t bytes_read = read(pipe_to_child[0], &number, sizeof(number));
        if (bytes_read == -1) {
            perror("read");
            close(pipe_to_child[0]);
            close(pipe_to_parent[1]);
            exit(EXIT_FAILURE);
        }

        printf("Дочерний процесс: получено число %d\n", number);

        // Вычисляем квадрат числа
        square = number * number;

        // Отправляем квадрат числа обратно родителю
        ssize_t bytes_written = write(pipe_to_parent[1], &square, sizeof(square));
        if (bytes_written == -1) {
            perror("write");
            close(pipe_to_child[0]);
            close(pipe_to_parent[1]);
            exit(EXIT_FAILURE);
        }

        printf("Дочерний процесс: отправлен квадрат числа %d\n", square);

        // Закрываем оставшиеся дескрипторы
        close(pipe_to_child[0]);
        close(pipe_to_parent[1]);

        exit(EXIT_SUCCESS);
    } else {
        // Код родительского процесса

        // Закрываем неиспользуемые концы каналов
        close(pipe_to_child[0]); // Закрываем чтение из канала родитель→дочерний
        close(pipe_to_parent[1]); // Закрываем запись в канал дочерний→родитель

        // Запрашиваем число у пользователя
        printf("Родительский процесс: введите число: ");
        scanf("%d", &number);

        // Отправляем число дочернему процессу
        ssize_t bytes_written = write(pipe_to_child[1], &number, sizeof(number));
        if (bytes_written == -1) {
            perror("write");
            close(pipe_to_child[1]);
            close(pipe_to_parent[0]);
            wait(NULL);
            exit(EXIT_FAILURE);
        }

        printf("Родительский процесс: отправлено число %d\n", number);

        // Ждём завершения дочернего процесса перед чтением ответа
        wait(NULL);

        // Читаем квадрат числа от дочернего процесса
        ssize_t bytes_read = read(pipe_to_parent[0], &square, sizeof(square));
        if (bytes_read == -1) {
            perror("read");
            close(pipe_to_child[1]);
            close(pipe_to_parent[0]);
            exit(EXIT_FAILURE);
        }

        printf("Родительский процесс: получен квадрат числа: %d\n", square);

        // Закрываем оставшиеся дескрипторы
        close(pipe_to_child[1]);
        close(pipe_to_parent[0]);
    }

    return 0;
}
