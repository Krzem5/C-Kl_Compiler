@echo off
cls
for /f "delims=" %%a in ('where python') do set PY_EXEC=%%a
if "%PY_EXEC%"=="" (
	echo Using Frozen Build...
	_frozenbuild\build %*
) else (
	echo Using Normal Build...
	python build.py %*
)
set PY_EXEC=
