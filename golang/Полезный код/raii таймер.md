```go
func TrackTime() func() {
  pre := time.Now()
  return func() {
    elapsed := time.Since(pre)
    fmt.Println("elapsed:", elapsed)
  }
}

func main() {
  ...
  defer TrackTime()()
  ...
}
```