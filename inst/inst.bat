@echo off
IF NOT EXIST ..\bin mkdir ..\bin
soul_inst -v ..\x64\Debug\*.exe ..\bin
soul_inst -v ..\x64\Release\*.exe ..\bin
