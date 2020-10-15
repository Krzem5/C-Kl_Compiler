@echo off
cls
cl -E src/core/*.c src/core/util/*.c -I src/include > preprocessed.c
