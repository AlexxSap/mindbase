
`Dependency Inversion Principle`, `DIP` — это один из пяти принципов `SOLID`, который описывает, как организовать зависимости между компонентами.

Согласно `DIP`, модули высокого уровня не должны зависеть от модулей низкого уровня, но оба должны зависеть от абстракций (интерфейсов в контексте Go). Другими словами, объект, зависящий от другого объекта, делегирует управление его жизненным циклом внешнему коду.

Допустим, у нас есть приложение, которое отправляет уведомления. 

#### Неправильный способ без использования DIP

```go
package main

import "fmt"

// Модуль низкого уровня
type EmailSender struct{}

func (es *EmailSender) SendEmail(message string) {
    fmt.Printf("Sending email: %s\n", message)
}

// Модуль высокого уровня
type NotificationService struct {
    emailSender *EmailSender
}

func NewNotificationService() *NotificationService {
    return &NotificationService{emailSender: &EmailSender{}}
}

func (ns *NotificationService) SendNotification(message string) {
    ns.emailSender.SendEmail(message)
}

func main() {
    notificationService := NewNotificationService()
    notificationService.SendNotification("Hello, world!")
}
```

В этом примере модуль высокого уровня `NotificationService` напрямую зависит от модуля низкого уровня `EmailSender`, что нарушает `DIP`.

#### Правильный способ с использованием DIP

```go
package main

import "fmt"

// Абстракция (интерфейс)
type MessageSender interface {
    Send(message string)
}

// Модуль низкого уровня
type EmailSender struct{}

func (es *EmailSender) Send(message string) {
    fmt.Printf("Sending email: %s\n", message)
}

// Модуль высокого уровня
type NotificationService struct {
    messageSender MessageSender
}

func NewNotificationService(ms MessageSender) *NotificationService {
    return &NotificationService{messageSender: ms}
}
```

В примере модуль высокого уровня `NotificationService` зависит от `MessageSender`.