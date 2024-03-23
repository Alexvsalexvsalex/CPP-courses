### Пререквезиты для локальной разработки:

* Устанавливем C/C++ compiler
* Устанавливаем https://conan.io версии 1.61.0 (можно поставить через [pip](https://docs.conan.io/en/latest/installation.html#install-with-pip-recommended))
* `conan profile new default --detect` данная команда смотрит на ваше окружение и сохраняет его в конфигурационный файл
* Клонируем ветку, переходим в папку с репозиторием
```shell
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j
./benchmark-cli
```
### Docker

Альтернативой является использование [Dockerfile](Dockerfile)
* `docker build -t hw-image - < Dockerfile`
* Запускаем контейнер
```shell
docker run -it --name hw2 -v ПУТЬ_ДО_ПАПКИ_С_РЕПО:/hw hw-image
cd /hw
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j
./benchmark-cli
```

При изменении исходного кода, вам будет достаточно запустить только команду `make -j`, находясь в директории `/hw/build`. При условии, что вы продолжаете работать в созданном контейнере.

#### Некоторые команды с пояснением.

Создаем контейнер на базе образа `hw-image` c примонтированной папкой. Задаем ему имя `hw2`. После создания, нас сразу перекинет во внутрь. Если хотим выйти, нажимаем `Ctrl + d`
```
docker run -it --name hw2 -v ПУТЬ_ДО_ПАПКИ_С_РЕПО:/hw hw-image
```
Команда `docker ps` или `docker ps -a` показывает запущенные или же все контейнеры.

Остановиь контейнер можно командой `docker stop hw2`. Повторно запустить контейнер можно командой `docker start hw2`.

Чтобы попасть внутрь контейнера, необходимо убедиться что он запущен, после этого выполнить команду `docker exec -it hw2 /bin/bash`

### Материал

На лекции 4 разбирали примитивы синхронизации. В конце лекции 5 разбирали std::condition_variable

[Код с лекции 4](https://github.com/akhoroshev/hpc-course-fall-2023/tree/lecture-4-cpp-mt)

[Код с лекции 5 про std::condition_variable](https://github.com/akhoroshev/hpc-course-fall-2023/tree/lecture-5-mpi/0-cond-var)

### Задание 1.1 Spinlock. 2.5 баллов

Реализовать примитив синхронизации spinlock. API уже объявлено в файле [spinlock.h](src/spinlock/spinlock.h), реализация в файле [spinlock.cpp](src/spinlock/spinlock.cpp). Можно добавлять/изменять privat'ную часть API как вам угодно.

### Задание 1.2 Spinlock tests. 2.5 баллов

Написать тесты на Spinlock. Используйте для этого файл [spinlock/test.cpp](test/spinlock/test.cpp).

### Задание 2.1 QueueWithSpinlock. 2.5 баллов

Реализовать многопоточную очередь, используя в качестве примитива синхронизации ваш Spinlock. API уже объявлено в файле [queue.h](src/queue/queue.h), реализация в файле [queue.cpp](src/queue/queue.cpp). Можно добавлять/изменять privat'ную часть API как вам угодно.

### Задание 2.2 QueueWithCondVar. 5 баллов

Реализовать многопоточную очередь, используя в качестве примитива синхронизации std::mutex и std::condition_variable. API уже объявлено в файле [queue.h](src/queue/queue.h), реализация в файле [queue.cpp](src/queue/queue.cpp). Можно добавлять/изменять privat'ную часть API как вам угодно.

### Задание 2.3 Queue tests. 2.5 баллов

Написать тесты на ваши очереди. Используйте для этого файл [queue/test.cpp](test/queue/test.cpp).

### Задание 3 Analyze. 5 баллов

Исслудуйте производительность полученных очередей с помощью [main.cpp](src/cli/main.cpp) и [benchmark.cpp](src/benchmark/benchmark.cpp).

### Как сдавать
* Делаете форк, у вас появится приватный репозиторий на github. Если форк уже есть, надо сделать git fetch upstream.
* Создаете ветку hw-2-dev как ветвь от hw-2
* Выполняете задание в ветке hw-2-dev
* Создаете PR в своем репо из hw-2-dev в hw-2 и ставите меня в review

### Deadline

* Soft -- 7.11.23 23:59
* Hard -- 14.11.22 23:59
