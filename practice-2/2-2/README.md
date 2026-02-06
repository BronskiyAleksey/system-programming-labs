Задание 2-2: Синхронизация с мьютексами
Требования:
•
Используйте pthread_mutex_t
•
Функции: pthread_mutex_init(), pthread_mutex_lock(), pthread_mutex_unlock(), pt
hread_mutex_destroy()
Вариант 5: Два потока пишут в файл shared.txt. Поток 1 пишет числа 1-50, поток 2 —
51-100. Мьютекс защищает файл
