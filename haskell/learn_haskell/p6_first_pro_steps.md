**Как создать простой проект шаг за шагом**

В каталоге проектов набираем
```shell
stack new firstpro simple
``` 
это создаст каталог шаблона простого проекта. Заглянем внутрь:
|- src
|- - Main.hs 
|- CHANGELOG.md
|- LICENSE
|- README.md
|- Setup.hs
|- firstpro.cabal
|- stack.yaml

Нас интересует файл `Main.hs` - это отправная точка для работы нашего проекта.
Внутри там написан простой `hello world`
```haskell
module Main (main) where

main :: IO ()
main = do
  putStrLn "hello world"
```

Соберём ...
```shell
stack build
```
... и запустим его
```shell
stack exec firstpro
``` 

Далее в файле `Main.hs` можно добавлять свои функции, добавлять свои модули в проект, но это уже совсем другая история.
