#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <time.h>
#include <string.h>


#define LOG_FILE "/tmp/daemon.log"
#define MONITORED_FILE "data.txt"
#define SLEEP_INTERVAL 5  // Проверка каждые 5 секунд

volatile sig_atomic_t keep_running = 1;

// Обработчик сигналов для корректного завершения
void signal_handler(int signum) {
    if (signum == SIGTERM || signum == SIGINT) {
        keep_running = 0;
    }
}

// Запись сообщения в лог с временной меткой
void log_message(const char *message) {
    FILE *log = fopen(LOG_FILE, "a");
    if (log) {
        time_t now = time(NULL);
        char *timestamp = ctime(&now);
        timestamp[strlen(timestamp) - 1] = '\0'; // Убираем символ новой строки
        fprintf(log, "[%s] %s\n", timestamp, message);
        fclose(log);
    }
}

int main() {
    pid_t pid, sid;

    // Шаг 1: Создаём дочерний процесс
    pid = fork();
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        exit(EXIT_SUCCESS); // Завершаем родительский процесс
    }

    // Шаг 2: Создаём новую сессию
    sid = setsid();
    if (sid < 0) {
        exit(EXIT_FAILURE);
    }

    // Шаг 3: Меняем рабочую директорию
    if (chdir("/") < 0) {
        exit(EXIT_FAILURE);
    }

    // Шаг 4: Устанавливаем маску создания файлов
    umask(0);

    // Шаг 5: Закрываем стандартные файловые дескрипторы
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    // Открываем /dev/null для перенаправления
    open("/dev/null", O_RDWR);  // stdin
    dup(0);  // stdout
    dup(0);  // stderr

    // Шаг 6: Настраиваем обработку сигналов
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);

    // Логируем запуск демона
    log_message("Демон запущен и начал мониторинг файла data.txt");

    off_t previous_size = -1;

    // Основной цикл мониторинга
    while (keep_running) {
        struct stat file_stat;
        if (stat(MONITORED_FILE, &file_stat) == 0) {
            off_t current_size = file_stat.st_size;
            if (current_size != previous_size) {
                char log_msg[256];
                snprintf(log_msg, sizeof(log_msg),
                        "Размер файла изменился: был %ld байт, стал %ld байт",
                        (long)previous_size, (long)current_size);
                log_message(log_msg);
                previous_size = current_size;
            }
        } else {
            // Файл не найден — логируем это событие
            if (previous_size != -1) {
                log_message("Файл data.txt не найден (возможно, удалён или перемещён)");
                previous_size = -1;
            }
        }
        sleep(SLEEP_INTERVAL);
    }

    // Логируем остановку демона
    log_message("Демон остановлен");
    exit(EXIT_SUCCESS);
}
