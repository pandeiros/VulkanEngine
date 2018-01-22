echo off
SETLOCAL ENABLEEXTENSIONS

call :build glslang
call :build spirv-tools
call :build Samples
call :build shaderc
echo Done.
goto EOF

:build
cd %VULKAN_SDK%\%1
echo Building %1
mkdir build
cd build
cmake -G "Visual Studio 15 2017 Win64" ..
msbuild ALL_BUILD.vcxproj /p:Platform=x64 /p:Configuration=Debug /verbosity:quiet
msbuild ALL_BUILD.vcxproj /p:Platform=x64 /p:Configuration=Release /verbosity:quiet
cd ..
exit /b

:EOF
