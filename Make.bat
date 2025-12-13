@echo off
    windres -i ".\lib\resourse.rc" -o ".\resourse.o"
    g++ ".\lib\main.cpp" -c ".\main.o"
    g++ ".\main.o" ".\resourse.o" -o ".\main.exe"