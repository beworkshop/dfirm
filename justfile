build:
    mkdir -p build
    cd build && cmake .. && make -j16
load:
    cp -v build/dfirm.uf2 /media/$USER/RP2350/
clean:
    cd build && rm -rf CMakeCache.txt CMakeFiles/ *.elf *.uf2 cmake_install.cmake Makefile
fullclean:
    rm -rf build/*
read:
    sudo picocom /dev/ttyACM0 -b 115200

update:
    cd build && cmake .. && make -j16
    cp -v build/dfirm.uf2 /media/$USER/RP2350/
    sleep 1
    sudo picocom /dev/ttyACM0 -b 115200