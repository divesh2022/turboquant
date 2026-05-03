@echo off
REM ============================================================
REM TurboQuant Black Box & White Box Test Runner
REM ============================================================

setlocal

REM Define paths
set SRC_DIR=src
set INCLUDE_DIR=include
set TEST_DIR=tests
set BUILD_DIR=build\tests

REM Ensure build directory exists
if not exist %BUILD_DIR% (
    mkdir %BUILD_DIR%
)

echo [STEP 1] Compiling test_blackbox_whitebox.cpp...
g++ -std=c++17 -O2 -Wall -I%INCLUDE_DIR% ^
    %TEST_DIR%\test_blackbox_whitebox.cpp ^
    %SRC_DIR%\core\qjl.cpp ^
    %SRC_DIR%\core\rotation.cpp ^
    %SRC_DIR%\transforms\polar.cpp ^
    -o %BUILD_DIR%\test_blackbox_whitebox.exe

if errorlevel 1 (
    echo [ERROR] Compilation failed.
    exit /b 1
)

echo [STEP 2] Running test_blackbox_whitebox.exe...
%BUILD_DIR%\test_blackbox_whitebox.exe

endlocal
