
Паттерн `Retry` (Повтор) учитывает возможный временный характер ошибки в распределенной системе и осуществляет повторные попытки выполнить неудачную операцию.

Любое взаимодействие программных компонентов ненадежно. Вызываемый компонент может быть временно недоступен или возвращать различные ошибки. Особенно если взаимодействие происходит по сети.

Некоторые типы компонентов обязаны быть устойчивы к временным сбоям, которые могут случаться в их окружении. Они должны иметь возможность повторять запросы или восстанавливать соединения.

## Компоненты

- `Effector` — функция, взаимодействующая с сервисом.
- `Retry` — функция, принимающая Effector.
## Пример кода

Сигнатура функции `Effector`:
```go
type Effector func(context.Context) (string, error)
```

Функция `Retry`:
```go
func Retry(effector Effector, retries int, delay time.Duration) Effector { 
    return func(ctx context.Context) (string, error) {

        for r := 0; ; r++ {
            response, err := effector(ctx) 
            if err == nil || r >= retries {
                return response, err 
            }

            log.Printf("Attempt %d failed; retrying in %v", r + 1, delay)
            select {
            case <-time.After(delay): 
            case <-ctx.Done():
                return "", ctx.Err() 
            }
        } 
    }
}
```

Возможно, вы сами заметили, почему функция `Retry` имеет такую стройную реализацию: она возвращает функцию, но у этой функции нет внешнего состояния.

Чтобы использовать `Retry`, необходимо реализовать функцию, которая выполняет потенциально неудачную операцию, чья сигнатура соответствует типу `Effector`. Выберем на эту роль функцию `EmulateTransientError`:

```go
var count int

func EmulateTransientError(ctx context.Context) (string, error) { 
    count++

    if count <= 3 {
        return "intentional fail", errors.New("error")
    } else {
        return "success", nil
    } 
}

func main() {
    r := Retry(EmulateTransientError, 5, 2*time.Second)
    res, err := r(context.Background())
    fmt.Println(res, err) 
}
```

Функция `main` передает функцию `EmulateTransientError` в `Retry` и сохраняет полученную функцию в переменной `r`. Когда будет вызвана функция `r`, она вызовет `EmulateTransientError` и повторит попытку после задержки, если `EmulateTransientError` вернет ошибку, в соответствии с логикой повтора, показанной выше. Наконец, после четвертой попытки `EmulateTransientError` возвращает nil в качестве ошибки и завершает работу.