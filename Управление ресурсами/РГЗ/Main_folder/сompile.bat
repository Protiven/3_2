if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" set value="C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat"
if exist %value% call %value% x86
cd /d %~dp0

echo Удаляются старые файлы
if exist *.exe del *.exe
if exist *.dll del *.dll

echo Компиляция
cl /EHsc RGZ_9.cpp kernel32.lib user32.lib gdi32.lib advapi32.lib /D "UNICODE" 
cl /LD  /D "UNICODE" Dll.cpp kernel32.lib user32.lib gdi32.lib advapi32.lib 
del *.obj *.lib *.exp
pause