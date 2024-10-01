[back](./lessons.md)
 
## Template Parameter for Lambdas

Теперь замыкания могут быть шаблонизированны 

```cpp
auto sumInt = [](int fir, int sec) { return fir + sec; };            // only to int convertible types    (C++11)
auto sumGen = [](auto fir, auto sec) { return fir + sec; };          // arbitrary types                  (C++14)
auto sumTem = []<typename T>(T fir, T sec) { return fir + sec; };    // arbitrary, but identical types   (C++20)

auto lambdaVector = []<typename T>(const std::vector<T>& vec) { return vec.size(); };
auto lambdaVectorIntegral = []<std::integral T>(const std::vector<T>& vec) { return vec.size(); };
```

Вроде ничо не изменилось, но:
- теперь можно явно указывать контейнер в котром лежит шаблонный тип
- можно использовать концепты, для ограничения специализации
