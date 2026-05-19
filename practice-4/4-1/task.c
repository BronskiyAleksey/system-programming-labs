#include <stdio.h>
#include <stdlib.h>

// Структура стека
typedef struct {
    int *data;      // Указатель на массив данных
    size_t size;    // Текущий размер стека
    size_t capacity; // Вместимость стека
} DynamicStack;

// Инициализация стека
DynamicStack* create_stack(size_t initial_capacity) {
    DynamicStack *stack = (DynamicStack*)malloc(sizeof(DynamicStack));
    if (stack == NULL) {
        fprintf(stderr, "Ошибка выделения памяти для стека\n");
        return NULL;
    }

    stack->data = (int*)calloc(initial_capacity, sizeof(int));
    if (stack->data == NULL) {
        fprintf(stderr, "Ошибка выделения памяти для данных стека\n");
        free(stack);
        return NULL;
    }

    stack->size = 0;
    stack->capacity = initial_capacity;
    return stack;
}

// Добавление элемента в стек (push)
int push(DynamicStack *stack, int value) {
    if (stack == NULL) {
        fprintf(stderr, "Стек не инициализирован\n");
        return -1;
    }

    // Если стек заполнен, увеличиваем его размер
    if (stack->size >= stack->capacity) {
        size_t new_capacity = stack->capacity * 2;
        int *new_data = (int*)realloc(stack->data, new_capacity * sizeof(int));
        if (new_data == NULL) {
            fprintf(stderr, "Ошибка перераспределения памяти\n");
            return -1;
        }
        stack->data = new_data;
        stack->capacity = new_capacity;
    }

    stack->data[stack->size] = value;
    stack->size++;
    return 0; // Успех
}

// Извлечение элемента из стека (pop)
int pop(DynamicStack *stack, int *value) {
    if (stack == NULL) {
        fprintf(stderr, "Стек не инициализирован\n");
        return -1;
    }

    if (stack->size == 0) {
        fprintf(stderr, "Стек пуст\n");
        return -1;
    }

    *value = stack->data[stack->size - 1];
    stack->size--;
    
    // Опционально: уменьшаем размер памяти, если занято мало места
    if (stack->size < stack->capacity / 4 && stack->capacity > 10) {
        size_t new_capacity = stack->capacity / 2;
        int *new_data = (int*)realloc(stack->data, new_capacity * sizeof(int));
        if (new_data != NULL) {
            stack->data = new_data;
            stack->capacity = new_capacity;
        }
    }
    
    return 0; // Успех
}

// Очистка стека и освобождение памяти
void free_stack(DynamicStack *stack) {
    if (stack != NULL) {
        if (stack->data != NULL) {
            free(stack->data);
        }
        free(stack);
    }
}

// Вспомогательная функция для вывода состояния стека
void print_stack(const DynamicStack *stack) {
    if (stack == NULL) {
        printf("Стек не инициализирован\n");
        return;
    }

    printf("Стек (размер: %zu, вместимость: %zu): ", stack->size, stack->capacity);
    for (size_t i = 0; i < stack->size; i++) {
        printf("%d ", stack->data[i]);
    }
    printf("\n");
}

// Пример использования
int main() {
    // Создаём стек с начальной вместимостью 5 элементов
    DynamicStack *stack = create_stack(5);
    if (stack == NULL) {
        return 1;
    }

    // Тестируем операции push
    printf("Добавляем элементы в стек:\n");
    for (int i = 1; i <= 8; i++) {
        if (push(stack, i * 10) == 0) {
            printf("Push: %d\n", i * 10);
            print_stack(stack);
        }
    }

    // Тестируем операции pop
    printf("\nИзвлекаем элементы из стека:\n");
    int value;
    for (int i = 0; i < 5; i++) {
        if (pop(stack, &value) == 0) {
            printf("Pop: %d\n", value);
            print_stack(stack);
        }
    }

    // Освобождаем память
    free_stack(stack);
    printf("Память освобождена.\n");

    return 0;
}
