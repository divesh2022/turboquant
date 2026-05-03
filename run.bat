@echo off
setlocal enabledelayedexpansion

echo ============================================
echo Starting TurboQuant Build Process
echo ============================================

set "SRC_DIR=src"
set "INCLUDE_DIR=include"
set "TEST_DIR=tests"
set "BUILD_DIR=build"

if not exist "%BUILD_DIR%\transforms" mkdir "%BUILD_DIR%\transforms"

echo [1/4] Compiling source files...
g++ -std=c++17 -O3 -march=native -I%INCLUDE_DIR% -c %SRC_DIR%\transforms\polar.cpp -o %BUILD_DIR%\transforms\polar.o
if %ERRORLEVEL% NEQ 0 (
    echo Compilation failed for polar.cpp
    exit /b %ERRORLEVEL%
)

echo [2/4] Compiling test suites...
g++ -std=c++17 -O3 -march=native -I%INCLUDE_DIR% -c %TEST_DIR%\test_qjl.cpp -o %BUILD_DIR%\test_qjl.o
g++ -std=c++17 -O3 -march=native -I%INCLUDE_DIR% -c %TEST_DIR%\test_turboquant.cpp -o %BUILD_DIR%\test_turboquant.o

echo [3/4] Linking files and creating executables...
:: The quotes around the linker flag protect the comma from PowerShell
g++ %BUILD_DIR%\test_qjl.o %BUILD_DIR%\transforms\polar.o -o %BUILD_DIR%\test_qjl.exe "-Wl,--subsystem,console"
if %ERRORLEVEL% NEQ 0 (
    echo Linking failed for test_qjl
    exit /b %ERRORLEVEL%
)

g++ %BUILD_DIR%\test_turboquant.o %BUILD_DIR%\transforms\polar.o -o %BUILD_DIR%\test_turboquant.exe "-Wl,--subsystem,console"
if %ERRORLEVEL% NEQ 0 (
    echo Linking failed for test_turboquant
    exit /b %ERRORLEVEL%
)

echo.
echo ============================================
echo Build Successful. Executing Test Suites...
echo ============================================
echo.

echo --- Running QJL Unit Tests ---
call %BUILD_DIR%\test_qjl.exe
echo.

echo --- Running TurboQuant Integration Tests ---
call %BUILD_DIR%\test_turboquant.exe
echo.

echo ============================================
echo All Operations Completed Successfully.
echo ============================================

endlocal