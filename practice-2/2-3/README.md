Задание 2-3: Обработка сигналов
Требования:
•
Используйте signal() или sigaction()
•
Обработчик должен быть async-signal-safe (используйте write() вместо printf())
•
Флаги: volatile sig_atomic_t
Вариант 5: Программа засыпает на 60 секунд sleep(60). Установить alarm(10). При
SIGALRM выводит "Alarm received". При SIGINT завершается
