@echo off
setlocal

if "%ECHSE_GENERIC%" == "" (
  echo Environment variable 'ECHSE_GENERIC' not set.
  pause
  exit
)

set script=%ECHSE_GENERIC%/scripts/echse_generate
if not exist %script% (
  echo Shell script '%script%' is missing.
  pause
  exit
)

bash -c "%script% %1"
if not errorlevel 0 (
  echo Error: Failed to run script '%script%'.
  echo 
  echo Also make sure that the bash interpreter is properly installed.
  echo On Windows, the interpreter is typically included with MSYS.
  pause
)

