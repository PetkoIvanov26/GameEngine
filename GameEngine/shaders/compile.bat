@echo off
setlocal

echo Starting shader compilation...

set GLSLC="%VULKAN_SDK%\Bin\glslc.exe"
set SHADER_DIR="%~dp0"

if not exist %GLSLC% (
    echo GLSLC compiler not found!
    exit /b 1
)

echo SHADER_DIR: %SHADER_DIR%

echo Compiling vertex shaders...
for %%f in (%SHADER_DIR%*.vert) do (
    echo Compiling %%f...
    %GLSLC% %%f -o %%~nf.vert.spv
    if errorlevel 1 (
        echo Error compiling %%f
        exit /b 1
    )
)
echo Compiling fragment shaders...
for %%f in (%SHADER_DIR%*.frag) do (
    echo Compiling %%f...
    %GLSLC% %%f -o %%~nf.frag.spv
    if errorlevel 1 (
        echo Error compiling %%f
        exit /b 1
    )
)

echo Shader compilation completed.
pause