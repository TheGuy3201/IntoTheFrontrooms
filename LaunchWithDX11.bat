@echo off
echo Launching Unreal Engine project with DX11 compatibility settings...
echo.
echo If menus still appear corrupted, try the following:
echo 1. In UE5 Editor, go to Edit > Project Settings
echo 2. Navigate to Platforms > Windows > Targeted RHIs
echo 3. Make sure DirectX 11 is selected and DirectX 12 is unchecked
echo 4. Restart the editor
echo.

start "" "C:\Program Files\Epic Games\UE_5.5\Engine\Binaries\Win64\UnrealEditor.exe" "%~dp0IntoTheFrontrooms.uproject" -dx11