# PascalCompiler

Компилятор упрощённой версии языка Pascal, выполненный на языке C++ с использованием ANTLR4
## Загрузка


## Сборка

### CMake 3.21+
### GNU Makefiles 
### Clang 10.0.4
### LLVM 10.0.0
### С++17 
### ANTLR 4.10.1


#### Debug:

```
cmake -S . -B build/debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build/debug
ctest --test-dir build/debug
```

#### Release:

```
cmake -S . -B build/release -DCMAKE_BUILD_TYPE=Release
cmake --build build/release
ctest --test-dir build/release
```

#### Common:
```
sh build.sh
```

## Запуск

```
./pascal-compiler <options> <input-file>
```
При запуске без опций генерируется исполняемый файл и сгенерированный LLVM IR в виде файла формата .ll
### Опции:

#### --file-path:
```
Указать путь к файлу
```
#### --dump-tokens:
```
Выводит все токены программы в стандартный выходной поток
```
Опция останавливает компиляцию программы после выполнения лексического анализа
#### --dump-ast:
```
Выводит AST-дерево в стандартный выходной поток в формате XML
```
Опция останавливает компиляцию программы после выполнения синтаксического анализа
#### --dump-asm:
```
Выводит сгенерированный LLVM IR в виде файла формата .ll
```
Опция останавливает компиляцию программы после генерации LLVM IR

