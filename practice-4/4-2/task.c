#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Использование: %s <лог-файл>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Ошибка открытия файла");
        return 1;
    }

    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        perror("Ошибка fstat");
        close(fd);
        return 1;
    }

    if (sb.st_size == 0) {
        printf("Файл пуст\n");
        close(fd);
        return 0;
    }

    char *mapped_file = (char *)mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (mapped_file == MAP_FAILED) {
        perror("Ошибка mmap");
        close(fd);
        return 1;
    }

    int error_count = 0;
    char *current = mapped_file;
    char *end = mapped_file + sb.st_size;

    while (current < end) {
        if (strncmp(current, "ERROR", 5) == 0 &&
            (current + 5 >= end || *(current + 5) == ' ' || *(current + 5) == '\t' || *(current + 5) == '\n')) {
            error_count++;
        }

        while (current < end && *current != '\n') {
            current++;
        }
        if (current < end) {
            current++;
        }
    }

    printf("Количество строк, начинающихся с ERROR: %d\n", error_count);

    if (munmap(mapped_file, sb.st_size) == -1) {
        perror("Ошибка munmap");
        close(fd);
        return 1;
    }

    close(fd);
    return 0;
}
