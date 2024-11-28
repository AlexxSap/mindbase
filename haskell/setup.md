## Установка

Устанавливаем GHCup (https://www.haskell.org/ghcup/)
```bash
curl --proto '=https' --tlsv1.2 -sSf https://get-ghcup.haskell.org | sh
```

## Настройка emacs

В файле init.el раскомментируем
```
(haskell +lsp +tree-sitter)    ; a language that's lazier than I am
```

## Запуск

Создаём проект (real это имя проекта)
```
stack new real
```

Собираем
```
stack build
```

Внутри каталога .stack-work появиться исполняемый файл, его можно запустить так
```
stack exec real-exe
```
