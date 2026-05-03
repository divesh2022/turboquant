@echo off
setlocal enabledelayedexpansion

REM ============================================================================
REM TurboQuant - Comprehensive Build and Execution Script
REM Compiles and executes all C++ files (excluding Python)
REM Respects proper dependency order and build sequence
REM ============================================================================

echo.
echo ============================================================================
echo              TurboQuant C++ Build and Execution Pipeline
echo ============================================================================
echo.

REM ============================================================================
REM Configuration
REM ============================================================================
set "PROJECT_DIR=%CD%"
set "SRC_DIR=%PROJECT_DIR%\src"
set "TESTS_DIR=%PROJECT_DIR%\tests"
set "BENCHMARK_DIR=%PROJECT_DIR%\benchmark"
set "INCLUDE_DIR=%PROJECT_DIR%\include"
set "BUILD_DIR=%PROJECT_DIR%\build"
set "OUTPUT_DIR=%BUILD_DIR%\output"

REM Compiler and Compiler Flags
set "CXX=g++"
set "CXXFLAGS=-std=c++17 -O3 -march=native -Wall -Wextra -pedantic"
set "INCLUDE_FLAGS=-I%INCLUDE_DIR%"

REM ============================================================================
REM Create Build Directories
REM ============================================================================
echo [STEP 1/6] Creating build directories...
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
if not exist "%BUILD_DIR%\src" mkdir "%BUILD_DIR%\src"
if not exist "%BUILD_DIR%\src\core" mkdir "%BUILD_DIR%\src\core"
if not exist "%BUILD_DIR%\src\transforms" mkdir "%BUILD_DIR%\src\transforms"
if not exist "%BUILD_DIR%\tests" mkdir "%BUILD_DIR%\tests"
if not exist "%BUILD_DIR%\benchmark" mkdir "%BUILD_DIR%\benchmark"
if not exist "%OUTPUT_DIR%" mkdir "%OUTPUT_DIR%"
echo [DONE] Build directories created.
echo.

REM ============================================================================
REM STEP 2: Compile Core Source Files (from src/core/)
REM ============================================================================
echo [STEP 2/6] Compiling core source files...
set "CORE_OBJECTS="

REM Check if core source files exist
if exist "%SRC_DIR%\core\*.cpp" (
    for %%F in ("%SRC_DIR%\core\*.cpp") do (
        echo   - Compiling %%~nF...
        %CXX% %CXXFLAGS% %INCLUDE_FLAGS% -c "%%F" -o "%BUILD_DIR%\src\core\%%~nF.o"
        if !ERRORLEVEL! NEQ 0 (
            echo [ERROR] Compilation failed for %%~nF
            goto ERROR_EXIT
        )
        set "CORE_OBJECTS=!CORE_OBJECTS! "%BUILD_DIR%\src\core\%%~nF.o""
    )
    echo [DONE] Core source files compiled.
) else (
    echo [INFO] No core source files found in %SRC_DIR%\core\
)
echo.

REM ============================================================================
REM STEP 3: Compile Transform Source Files (from src/transforms/)
REM ============================================================================
echo [STEP 3/6] Compiling transform source files...
set "TRANSFORM_OBJECTS="

if exist "%SRC_DIR%\transforms\*.cpp" (
    for %%F in ("%SRC_DIR%\transforms\*.cpp") do (
        echo   - Compiling %%~nF...
        %CXX% %CXXFLAGS% %INCLUDE_FLAGS% -c "%%F" -o "%BUILD_DIR%\src\transforms\%%~nF.o"
        if !ERRORLEVEL! NEQ 0 (
            echo [ERROR] Compilation failed for %%~nF
            goto ERROR_EXIT
        )
        set "TRANSFORM_OBJECTS=!TRANSFORM_OBJECTS! "%BUILD_DIR%\src\transforms\%%~nF.o""
    )
    echo [DONE] Transform source files compiled.
) else (
    echo [INFO] No transform source files found in %SRC_DIR%\transforms\
)
echo.

REM ============================================================================
REM STEP 4: Compile and Link Unit Tests
REM ============================================================================
echo [STEP 4/6] Compiling and linking unit tests...
echo   - Compiling test_qjl.cpp...
%CXX% %CXXFLAGS% %INCLUDE_FLAGS% -c "%TESTS_DIR%\test_qjl.cpp" -o "%BUILD_DIR%\tests\test_qjl.o"
if !ERRORLEVEL! NEQ 0 (
    echo [ERROR] Compilation failed for test_qjl.cpp
    goto ERROR_EXIT
)

echo   - Linking test_qjl executable...
%CXX% %CXXFLAGS% "%BUILD_DIR%\tests\test_qjl.o" %CORE_OBJECTS% %TRANSFORM_OBJECTS% -o "%BUILD_DIR%\tests\test_qjl.exe"
if !ERRORLEVEL! NEQ 0 (
    echo [ERROR] Linking failed for test_qjl
    goto ERROR_EXIT
)

echo   - Compiling test_turboquant.cpp...
%CXX% %CXXFLAGS% %INCLUDE_FLAGS% -c "%TESTS_DIR%\test_turboquant.cpp" -o "%BUILD_DIR%\tests\test_turboquant.o"
if !ERRORLEVEL! NEQ 0 (
    echo [ERROR] Compilation failed for test_turboquant.cpp
    goto ERROR_EXIT
)

echo   - Linking test_turboquant executable...
%CXX% %CXXFLAGS% "%BUILD_DIR%\tests\test_turboquant.o" %CORE_OBJECTS% %TRANSFORM_OBJECTS% -o "%BUILD_DIR%\tests\test_turboquant.exe"
if !ERRORLEVEL! NEQ 0 (
    echo [ERROR] Linking failed for test_turboquant
    goto ERROR_EXIT
)
echo [DONE] Unit tests compiled and linked.
echo.

REM ============================================================================
REM STEP 5: Compile and Link Benchmark
REM ============================================================================
echo [STEP 5/6] Compiling and linking benchmark...
echo   - Compiling benchmark_quant.cpp...
%CXX% %CXXFLAGS% %INCLUDE_FLAGS% -c "%BENCHMARK_DIR%\benchmark_quant.cpp" -o "%BUILD_DIR%\benchmark\benchmark_quant.o"
if !ERRORLEVEL! NEQ 0 (
    echo [ERROR] Compilation failed for benchmark_quant.cpp
    goto ERROR_EXIT
)

echo   - Linking benchmark executable...
%CXX% %CXXFLAGS% "%BUILD_DIR%\benchmark\benchmark_quant.o" %CORE_OBJECTS% %TRANSFORM_OBJECTS% -o "%BUILD_DIR%\benchmark\benchmark_quant.exe"
if !ERRORLEVEL! NEQ 0 (
    echo [ERROR] Linking failed for benchmark_quant
    goto ERROR_EXIT
)
echo [DONE] Benchmark compiled and linked.
echo.

REM ============================================================================
REM STEP 6: Execute All Compiled Binaries in Proper Order
REM ============================================================================
echo [STEP 6/6] Executing compiled binaries...
echo.
echo ============================================================================
echo                     EXECUTION PHASE - Test Results
echo ============================================================================
echo.

set "TEST_FAILED=0"

REM Execute Unit Tests First
echo --- Running Unit Tests: test_qjl ---
if exist "%BUILD_DIR%\tests\test_qjl.exe" (
    "%BUILD_DIR%\tests\test_qjl.exe"
    if !ERRORLEVEL! NEQ 0 (
        echo [FAILED] test_qjl returned error code !ERRORLEVEL!
        set "TEST_FAILED=1"
    ) else (
        echo [PASSED] test_qjl executed successfully.
    )
) else (
    echo [SKIPPED] test_qjl.exe not found
)
echo.

echo --- Running Integration Tests: test_turboquant ---
if exist "%BUILD_DIR%\tests\test_turboquant.exe" (
    "%BUILD_DIR%\tests\test_turboquant.exe"
    if !ERRORLEVEL! NEQ 0 (
        echo [FAILED] test_turboquant returned error code !ERRORLEVEL!
        set "TEST_FAILED=1"
    ) else (
        echo [PASSED] test_turboquant executed successfully.
    )
) else (
    echo [SKIPPED] test_turboquant.exe not found
)
echo.

echo --- Running Performance Benchmark: benchmark_quant ---
if exist "%BUILD_DIR%\benchmark\benchmark_quant.exe" (
    "%BUILD_DIR%\benchmark\benchmark_quant.exe"
    if !ERRORLEVEL! NEQ 0 (
        echo [FAILED] benchmark_quant returned error code !ERRORLEVEL!
        set "TEST_FAILED=1"
    ) else (
        echo [PASSED] benchmark_quant executed successfully.
    )
) else (
    echo [SKIPPED] benchmark_quant.exe not found
)
echo.

REM ============================================================================
REM Summary and Exit
REM ============================================================================
echo ============================================================================
echo                              Build Summary
echo ============================================================================
echo Build Directory:    %BUILD_DIR%
echo Output Directory:   %OUTPUT_DIR%
echo Include Path:       %INCLUDE_DIR%
echo.

if !TEST_FAILED! EQU 0 (
    echo [SUCCESS] All build and execution steps completed successfully!
    echo.
    echo Generated Executables:
    if exist "%BUILD_DIR%\tests\test_qjl.exe" echo   - %BUILD_DIR%\tests\test_qjl.exe
    if exist "%BUILD_DIR%\tests\test_turboquant.exe" echo   - %BUILD_DIR%\tests\test_turboquant.exe
    if exist "%BUILD_DIR%\benchmark\benchmark_quant.exe" echo   - %BUILD_DIR%\benchmark\benchmark_quant.exe
    echo.
    echo To clean build artifacts, run: rmdir /s /q %BUILD_DIR%
    echo.
    goto SUCCESS_EXIT
) else (
    echo [WARNING] Some tests or benchmarks failed. Review output above.
    echo.
    goto ERROR_EXIT
)

REM ============================================================================
REM Error Exit Handler
REM ============================================================================
:ERROR_EXIT
echo.
echo ============================================================================
echo                          BUILD FAILED
echo ============================================================================
echo An error occurred during compilation or execution.
echo Check the messages above for details.
echo.
endlocal
exit /b 1

REM ============================================================================
REM Success Exit Handler
REM ============================================================================
:SUCCESS_EXIT
echo ============================================================================
echo.
endlocal
exit /b 0
