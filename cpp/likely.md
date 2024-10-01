[back](./lessons.md)

## Нравится - не нравится
__или вероятно - невероятно__

Аттрибуты можно использовать в метках и выражениях. Они являются подсказками для компилятора - какой путь выражения наиболее частый.

Хорошие примеры:
```cpp
constexpr long long fact(long long n) noexcept
{
    if (n > 1) [[likely]]
        return n * fact(n - 1);
    else [[unlikely]]
        return 1;
}
```

```cpp
switch (baz) {
    [[likely]] case 0: whatever(); break;
    [[unlikely]] case 1: something_else(); break;
    default: break;
}
```

Плохие примеры:
```cpp
if (something) {
  [[likely]];
  [[unlikely]];
  foo(something);
}
```

```cpp
if (foo) [[likely]] { // A
  baz();
} else if (bar) [[likely]] { // B
  quux();
} else [[unlikely]] { // C
  bing();
}
```

## Минусы
- нет контроля со стороны языка того, как это должно использоваться
- непонятно как что себя ведёт в плохих примерах, не интуитивно
- не понятно, оптимизирует ли это что-то вообще - зависит от конкретного компилятора
