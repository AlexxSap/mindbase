[back](./lessons.md)

## Концепты 

Концепты позволяют установить ограничения для параметров шаблонов. 

Пример:
```cpp
template <typename T>
concept size = sizeof(T) <= sizeof(int);

template <size T>
T sum(T a, T b)
{
    return a + b;
}
```
В данном случае определен концепт size. Его смысл в том, что тип, который будет передаваться через параметр T, должен удовлетворять условию sizeof(T) <= sizeof(int). То есть физический размер объектов типа T не должен быть больше размера значений типа int.

```cpp
template <class T>
concept Integral = std::is_integral<T>::value;

template <class T>
concept SignedIntegral = Integral<T> &&
                         std::is_signed<T>::value;
template <class T>
concept UnsignedIntegral = Integral<T> &&
                           !SignedIntegral<T>;
```

### Зачем?
- не так страшно выглядит как куча enadle_if, SFINAE
- понятные сообщения об ошибках
- можно вынесте ограничения в отдельную сущность и использовать в разных функциях/классах
 
# [еще пример](~/projects/concepts/main.cpp)


