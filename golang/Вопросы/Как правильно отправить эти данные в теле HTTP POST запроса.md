
```go
data := "test data"
```

Для отправки данных в теле `HTTP POST` запроса важно знать тип содержимого. Поскольку это сырой текст, мы будем использовать тип содержимого `text/plain`. Функция `http.Post` требует `io.Reader` в качестве тела, а не строки или байтов:
```go
Post(url string, contentType string, body io.Reader) (resp *http.Response, err error)
```

Интерфейс `Reader` определен следующим образом:
```go
type Reader interface {
    Read(p []byte) (n int, err error)
}
```

Чтобы соответствовать требованиям, мы преобразуем тело в буфер, который реализует этот интерфейс:
```go
func main() {
    data := "test data"
  
    contentType := "text/plain"
  
    body := strings.NewReader(data)
    // или 
    // body := bytes.NewBufferString(data)
  
    resp, err := http.Post("https://example.com", contentType, body)
 
    // ....
}
```