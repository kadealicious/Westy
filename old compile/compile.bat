@echo off
setlocal enabledelayedexpansion

set /p buildnum= 0<ver.txt
set /a newbuild=%buildnum%+1
echo %newbuild% 1>ver.txt

set filename=build_%buildnum%
for /l %%a in (1,1,31) do if "!filename:~-1!"==" " set filename=!filename:~0,-1!
echo file will be written to: %filename%

@echo on
gcc -std=c99 -o builds/"%filename%.exe" main.c graphics.c -lmingw32 -lglfw3dll -lglew32 -lopengl32

@echo off
pause