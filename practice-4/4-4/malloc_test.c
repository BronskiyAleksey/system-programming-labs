// malloc_test.c
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>  // Добавляем для функции sleep()

int main() {
    printf("Allocating memory...\n");
    
    // Несколько вызовов malloc для разных размеров
    void *ptr1 = malloc(1024);      // 1 KB
    void *ptr2 = malloc(4096);      // 4 KB
    void *ptr3 = malloc(65536);     // 64 KB
    
    printf("Memory allocated, sleeping for 10 seconds...\n");
    sleep(10);  // Даём время для strace
    
    free(ptr1);
    free(ptr2);
    free(ptr3);
    
    printf("Memory freed, exiting.\n");
    return 0;
}
