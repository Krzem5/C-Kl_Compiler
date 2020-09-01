@echo off
cls
set _INCLUDE=%INCLUDE%
set INCLUDE=.\src;.\src\include;%INCLUDE%
set PLATFORM=windows
if "%~1"=="" (
	strace build/kl.exe test.kl
) else (
	cl /E /permissive- /GS /W4 /Zc:wchar_t /ZI /Gm- /Od /sdl /Zc:inline /fp:precise /D "_DEBUG" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /errorReport:none /WX /Zc:forScope /RTC1 /Gd /MDd /FC /EHsc /nologo /diagnostics:column src\main.c src\core\platform\%PLATFORM%.c src\core\*.c src\core\util\*.c > preprocessed.c
)
set INCLUDE=%_INCLUDE%
