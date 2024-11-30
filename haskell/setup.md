## Установка

Устанавливаем GHCup (https://www.haskell.org/ghcup/)
```bash
curl --proto '=https' --tlsv1.2 -sSf https://get-ghcup.haskell.org | sh
```

В файле .bashrc добавляем в PATH каталог .ghcup/bin

Устанавливаем ghc-mod
```
stack install ghc-mod
```

Устанавливаем lsp
```
yay -S haskell-language-server-static
```

Устанавливаем форматтер
```
stack install fourmolu
```

## Настройка emacs

В файле init.el раскомментируем
```
(haskell +lsp +tree-sitter)    ; a language that's lazier than I am
```

В config.el добавляем
```
(require 'lsp)
(require 'lsp-haskell)
;; Hooks so haskell and literate haskell major modes trigger LSP setup
(add-hook 'haskell-mode-hook #'lsp)
(add-hook 'haskell-literate-mode-hook #'lsp)
(setq haskell-stylish-on-save t)
```

## Запуск

Создаём проект (real это имя проекта)
```
stack new real
```

Для создания проекта можно использовать схему simple - она создаёт меньше файлов в проекте.
```
stack new real simple
```

Собираем
```
stack build
```

Внутри каталога .stack-work появиться исполняемый файл, его можно запустить так
```
stack exec real-exe
```

Форматировать файл 
```
fourmolu -i Module.hs
```

Форматировать каталог 
```
fourmolu -i src
```
