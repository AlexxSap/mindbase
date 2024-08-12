`io.TeeReader` в Go используется для одновременного чтения данных из `io.Reader` и копирования этих данных в `io.Writer`. Это особенно полезно при логировании содержимого HTTP-запросов в мидлваре, позволяя одновременно читать и логировать данные, не теряя их для последующей обработки.

Преимущества использования `io.TeeReader` включают возможность прозрачного логирования данных запроса без изменения основного потока чтения данных. Ограничение заключается в том, что использование `io.TeeReader` может привести к незначительному увеличению задержек из-за дополнительной операции записи во время чтения данных.

Простой пример:
```go
package main

import (
    "fmt"
    "io"
    "io/ioutil"
    "log"
    "net/http"
    "strings"
)

func logRequestData(next http.Handler) http.Handler {
    return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
        // Создание буфера для логирования данных запроса
        var buf strings.Builder
        teeReader := io.TeeReader(r.Body, &buf)

        // Чтение и логирование содержимого запроса
        body, err := ioutil.ReadAll(teeReader)
        if err != nil {
            http.Error(w, "Ошибка при чтении тела запроса", http.StatusInternalServerError)
            return
        }
        
        // Запись данных обратно в тело запроса для последующей обработки
        r.Body = ioutil.NopCloser(strings.NewReader(buf.String()))

        // Вывод тела запроса в лог
        log.Printf("Received request with body: %s", body)

        // Продолжение обработки запроса
        next.ServeHTTP(w, r)
    })
}

func mainHandler(w http.ResponseWriter, r *http.Request) {
    fmt.Fprintf(w, "Запрос обработан")
}

func main() {
    finalHandler := http.HandlerFunc(mainHandler)
    http.Handle("/", logRequestData(finalHandler))
    log.Println("Сервер запущен на порту 8080")
    http.ListenAndServe(":8080", nil)
}
```

В примере мы создаем мидлваре, которая использует `io.TeeReader` для чтения тела HTTP-запроса, одновременно логируя его в stdout. Это позволяет сохранить неизменным первоначальный поток данных запроса для дальнейшей обработки в последующих обработчиках.