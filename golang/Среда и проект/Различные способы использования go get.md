Вы, конечно, знаете, как получить последнюю версию модуля:
Либо для конкретного модуля...
```
$ go get golang.org/x/net
```

...либо для всех зависимостей главного модуля:
```
$ go get -u ./...
```

Чтобы обновить все зависимости до последних патч-релизов текущей версии, используйте:
```
$ go get -u=patch ./...
```

Но это еще не все. Например, вы можете обновить или понизить версию модуля до конкретной версии:
```
$ go get golang.org/x/text@v0.3.2
```

Вы не ограничены использованием семантического номера версии. Вы можете использовать @latest, чтобы обновиться до последней версии, или @patch, чтобы обновиться до последнего патч-релиза. Или можете указать конкретную ветку VCS. 

Например, эта команда обновит модуль до последнего коммита в главной ветке модуля:
```
$ go get golang.org/x/text@main
```

Вы даже можете удалить зависимость. Эта операция даже понижает версии модулей:
```
$ go get golang.org/x/text@none
```


Команда go get может быть применена даже к инструментарию Go. Эта команда обновляет минимально требуемую версию Go для главного модуля (то есть обновляет директиву go в go.mod) и даже загружает последнюю или указанную версию инструментария, если это необходимо:
```
$ go get go
$ go get go@v1.21.4
```

Начиная с Go 1.21, go mod также может предложить версию инструментария Go для компиляции модуля, используя директиву toolchain (в отличие от директивы go, которая устанавливает минимально требуемую версию Go для модуля). Директива go остается неизменной. Как и в случае с go get, вы можете использовать @patch, чтобы обновиться до последнего патч-релиза текущего инструментария Go.
```
$ go get toolchain
$ go get toolchain@v1.21.4
$ go get toolchain@patch
```
