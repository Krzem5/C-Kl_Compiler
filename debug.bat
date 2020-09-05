@echo off
cls
set _INCLUDE=%INCLUDE%
set INCLUDE=.\src;.\src\include;%INCLUDE%
if "%~1"=="" (
	cd build
	strace kl.exe ../test.kl
	cd ../
) else (
	cl /E /permissive- /GS /W4 /Zc:wchar_t /ZI /Gm- /Od /sdl /Zc:inline /fp:precise /D "_DEBUG" /D "_USRDLL" /D "_WINDLL" /D "_WINDOWS" /D "_UNICODE" /D "UNICODE" /errorReport:none /WX /Zc:forScope /RTC1 /Gd /MDd /FC /EHsc /nologo /diagnostics:column src\dllmain.c src\core\platform\windows.c src\core\*.c src\core\util\*.c src\core\types\*.c > preprocessed.c
)
set INCLUDE=%_INCLUDE%
