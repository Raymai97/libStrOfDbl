@echo off
pushd %~dp0\..
if not exist build md build || goto err
gcc -O0 -o "%cd%\build\gcc_c89.exe" ^
	"%cd%\src\main.c" ^
	-I"%cd%\..\client" -DTEST_NO_I64 -DTEST_NO_LLONG ^
	-Wall -Wextra -std=c89 -pedantic ^
	-Wstrict-prototypes ^
	-Wold-style-definition || goto err
popd
exit/b

:err
pause
exit/b 1
