# Hillis-Steele inclusive scan

### Как собрать и запустить

(потребуется установленный OpenCL на машине, если не очень хотите ставить его себе в систему, то напишите мне свою конфигурацию, сделаем Docker)

```
mkdir build && cd build
cmake ..
make && make test
```

### Задача

Дописать код в файле `src/scan.cl` в функции `scan` (там где TODO)


### Сборка в докере intel (экспериментально)

```
docker build -t opencl-intel .
docker run -it --rm -v ПУТЬ_ДО_ПАПКИ_С_ДЗ:/code opencl-intel bash
cd /code
mkdir build && cd build
cmake .. -DOpenCL_LIBRARY=$OCL_LIB/libOpenCL.so.2.0 -DOpenCL_INCLUDE_DIR=$OCL_INC/
make
```
