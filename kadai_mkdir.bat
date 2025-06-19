@echo off
setlocal EnableDelayedExpansion
title kadai フォルダの作成

cls
cd "%~dp0"
echo;

set mkdirs=
:check
set /p mkdirs="作成するフォルダ数を入力："
if "%mkdirs%"=="" goto :check

cls
echo;
for /l %%i in (1,1,!mkdirs!) do (
  mkdir kadai%%i
  echo f | xcopy Inspector\mono2025.h kadai%%i\mono2025.h /y /h /q > nul
  :attrib -h kadai%%i\mono2025.h>nul
  echo f | xcopy temp.c kadai%%i\kadai%%i.ino /y /h /q > nul
  attrib -h kadai%%i\kadai%%i.ino>nul
  if %ErrorLevel% equ 0 echo kadai%%i を作成しました。
  echo;
)
attrib +h temp.c > nul
attrib +h kadai_mkdir.bat > nul

start explorer "%~dp0"
start Inspector\Inspector.ino
echo;
timeout /t 3 /nobreak
