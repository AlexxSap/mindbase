[back](./lessons.md)

# designated initializers

Как и трехсторонее сравнение просто тиснуто из языка C.

Можно использовать для класса класса, который:
- не имеет private или protected прямых нестатических элементов данных
- не имеет объявленных пользователем или унаследованных конструкторов
- не имеет виртуальных, private или protected базовых классов
- не имеет виртуальных функций-членов

Ну то есть простые структуры только

## Минусы
- Порядок инициализаторов должен быть как в объявлении
- Нельзя инициализировать вложенные объекты

## Плюсы
- более читабельно, если лень писать бидер

Примеры:
```cpp
struct Point { 
    double x { 0.0 };
    double y { 0.0 };
};

const Point p { .x = 10.0, .y = 20.0 };
```

```cpp
struct ScreenConfig {
    bool autoScale { false };
    bool fullscreen { false };
    int bits { 24 };
    int planes { 2 };
};

ScreenConfig cfg { true, false, 8, 1 }; 

ScreenConfig playbackCfg {
    .autoScale = true, .fullscreen = false, .bits = 8, .planes = 1
};
```
