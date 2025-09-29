@echo off
echo Launching Unreal Editor with DDC Memory Cache override...
echo This will bypass DDC configuration issues during startup.
echo.

"D:\UE\UE_5.6\Engine\Binaries\Win64\UnrealEditor.exe" "%~dp0DarkAge.uproject" -DDC-ForceMemoryCache

pause