FROM gcc:10

RUN  apt update && apt install -y \
     git \
     python3 \
     python3-pip \
     cmake

RUN  pip3 install conan==1.61.0 && \
     pip3 install matplotlib==3.8.0 && \
     conan profile new default --detect && \
     conan profile update options.boost:without_stacktrace=True default
