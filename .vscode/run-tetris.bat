@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64
if errorlevel 1 exit /b %errorlevel%
cl.exe /EHsc /Zi /MDd /std:c++17 /I "%~dp0..\SFML\include" "%~dp0..\main.cpp" "%~dp0..\functionality.cpp" /link /LIBPATH:"%~dp0..\SFML\lib" sfml-graphics-d.lib sfml-window-d.lib sfml-system-d.lib /OUT:"%~dp0..\tetris_game.exe"
if errorlevel 1 exit /b %errorlevel%
start "Tetris" "%~dp0..\tetris_game.exe"
