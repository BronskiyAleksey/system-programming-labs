Задание 2-1: Многопоточность
Требования:
•
Используйте библиотеку pthread
•
Компиляция: gcc -pthread -o task task.c
•
Функции: pthread_create(), pthread_join(), pthread_exit()
Вариант 5: Создать 2 потока. Первый пишет в output.txt числа 1-100. Второй читает
из input.txt и подсчитывает строки. Использовать pthread_join()
