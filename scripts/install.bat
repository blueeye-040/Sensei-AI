@echo off
:: ============================================================
::  Invincible Assistant - Simple Installer
::  Run this once on your Windows machine.
::  Does NOT need Administrator rights.
:: ============================================================

echo.
echo  Installing Invincible Assistant...
echo.

:: %USERPROFILE% = C:\Users\YourName  (works without admin rights)
set "INSTALL_DIR=%USERPROFILE%\InvincibleAssistant"

:: Find the exe — look next to this script first, then in ..\build\
set "EXE=%~dp0GhostUI.exe"
if not exist "%EXE%" set "EXE=%~dp0..\build\GhostUI.exe"

if not exist "%EXE%" (
    echo  [ERROR] GhostUI.exe not found.
    echo  Put GhostUI.exe in the same folder as install.bat, then run again.
    echo.
    pause
    exit /b 1
)

:: Create the install folder if it doesn't exist
mkdir "%INSTALL_DIR%" 2>nul

:: Copy the exe into the install folder
copy /Y "%EXE%" "%INSTALL_DIR%\GhostUI.exe" >nul

:: Create a Desktop shortcut using PowerShell (no admin needed)
set "SHORTCUT=%USERPROFILE%\Desktop\Invincible Assistant.lnk"
powershell -Command ^
  "$s=(New-Object -COM WScript.Shell).CreateShortcut('%SHORTCUT%'); $s.TargetPath='%INSTALL_DIR%\GhostUI.exe'; $s.Save()"

echo  [OK] App installed to:  %INSTALL_DIR%
echo  [OK] Desktop shortcut:  Invincible Assistant.lnk
echo.
echo  HOW TO USE:
echo    1. Double-click "Invincible Assistant" on your Desktop
echo    2. A small window opens with START and STOP buttons
echo    3. Press START  -> the invisible box appears on screen
echo    4. Press STOP   -> the box disappears
echo    5. Drag the box to reposition it anywhere on screen
echo    6. The box is visible to YOU but NOT to screen share/recordings
echo.
pause
