
https://doc.rust-lang.org/cargo/index.html

Чтобы создать проект с именем name нужно
```
cargo new name --bin
```
`--bin` значит что нужен исполняемый файл, для библиотеки нужно использовать`--lib`

`cargo build`  сборка проекта в дэбаге, для релиза нужно добавить`--release`
`cargo run` - запуск

Можно создать проект без добавления в него git'а
```
cargo new название --vcs none
```
после `--vcs` можно указать 
### cargo-edit
Упрощает установку зависимостей
Устанавливаем
```
cargo install cargo-edit
```

Добавляем зависимость (например крейт num)
```
cargo add num
```