cmake_minimum_required(VERSION 3.30)
project(sysopy_laby C)

set(CMAKE_C_STANDARD 17)

add_executable(sysopy_laby
        lab1/countdown.c
        lab2/collatz.c
        lab2/collatz.h
        lab2/client.c
        lab3/flipper_prev_version.c
        lab3/flipper.c
        lab4/zad1.c
        lab4/zad2.c
        lab5/zad1.c
        lab5/catcher.c
        lab5/sender.c
        lab6/zad1.c
        lab6/zad2_1.c
        lab7/client.c
        lab7/server.c
        lab8/user.c
        lab8/printer.c
        lab8/init.c
        lab9/zad1.c
        lab10/zad1.c
        lab11/client.c
        lab11/server.c
        lab12/client.c
        lab12/server.c
)
