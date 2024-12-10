# Как использовать пакет context в Go?

Пакет `context` помогает управлять временем выполнения и отменой операций, особенно в веб-приложениях и микросервисах. Это необходимо для предотвращения зависаний и утечек ресурсов

### Создание контекста с тайм-аутом

Контекст с тайм-аутом автоматически отменяет операцию, если она не завершилась за указанное время
Пример:
```go
package main

import (
  "context"
  "fmt"
  "time"
)

func doWork(ctx context.Context) {
  select {
  case <-time.After(2 * time.Second):
    fmt.Println("Work completed")
  case <-ctx.Done():
    fmt.Println("Work canceled:", ctx.Err())
  }
}

func main() {
  ctx, cancel := context.WithTimeout(context.Background(), 1*time.Second)
  defer cancel() // Освобождаем ресурсы

  go doWork(ctx)

  time.Sleep(2 * time.Second)
  fmt.Println("Done")
}
```

Что здесь происходит:
- `context.WithTimeout` создаёт контекст, который завершится через 1 секунду.
- Функция `doWork` проверяет канал `ctx.Done()` для отмены.

Результат: работа прерывается через 1 секунду.

### Контекст с дедлайном

Дедлайн задаёт фиксированное время завершения.
```go
ctx, cancel := context.WithDeadline(context.Background(), time.Now().Add(2*time.Second))
defer cancel()
```
Это полезно для заданий, где нужно строгое время завершения

### Встраивание значений в контекст

Контекст может передавать данные между частями программы. Используйте context.WithValue:
```go
func doWork(ctx context.Context) {
  userID := ctx.Value("userID")
  fmt.Println("User ID:", userID)
}

func main() {
  ctx := context.WithValue(context.Background(), "userID", 42)
  doWork(ctx)
}
```
Когда использовать:
- Передача метаданных, например, ID пользователя, токенов аутентификации.
- Не используйте контекст для передачи больших объёмов данных.

### Прерывание нескольких горутин

Контекст позволяет координировать отмену между несколькими горутинами:
```go
func worker(ctx context.Context, id int) {
  for {
    select {
    case <-ctx.Done():
      fmt.Printf("Worker %d stopped\n", id)
      return
    default:
      fmt.Printf("Worker %d working\n", id)
      time.Sleep(500 * time.Millisecond)
    }
  }
}

func main() {
  ctx, cancel := context.WithCancel(context.Background())

  for i := 1; i <= 3; i++ {
    go worker(ctx, i)
  }

  time.Sleep(2 * time.Second)
  cancel() // Отменяем все горутины
  time.Sleep(1 * time.Second)
  fmt.Println("All workers stopped")
}
```
Что происходит:
- Отмена контекста (cancel) сигнализирует всем горутинам остановиться через `ctx.Done()`.

### Советы:

- Используйте `context` только для управления временем выполнения и метаданными. Не передавайте через контекст сложные данные.
- Добавляйте тайм-ауты ко всем сетевым операциям. Это помогает избежать зависания при проблемах с соединением.
- Обязательно вызывайте `cancel()` для освобождения ресурсов
