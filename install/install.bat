@echo off
IF NOT EXIST ..\bin mkdir ..\bin
xcopy /Y ..\x64\Debug\*.exe ..\bin
xcopy /Y ..\x64\Release\*.exe ..\bin
