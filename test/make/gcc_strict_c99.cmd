@echo off
pushd %~dp0\..
if not exist build md build || goto err
gcc -O0 -o "%cd%\build\gcc_c99.exe" ^
	"%cd%\src\main.c" ^
	-I"%cd%\..\client" ^
	-Wall -Wextra -std=c99 -pedantic ^
	-Wstrict-prototypes ^
	-Wold-style-definition || goto err
popd
exit/b

:err
pause
exit/b 1
