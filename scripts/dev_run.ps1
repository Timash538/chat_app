$ErrorActionPreference = "Stop"
$projectRoot = $PSScriptRoot -replace '\\scripts$', ''
$binDir = "$projectRoot\build\x64-Debug\bin"

$serverExe = "$binDir\server.exe"
$clientExe = "$binDir\client.exe"
$composeFile = "$projectRoot\docker-compose.yml"

# --- Очистка ---
Write-Host "[+] Cleaning up..."
Get-Process server -ErrorAction SilentlyContinue | Stop-Process -Force
Get-Process client -ErrorAction SilentlyContinue | Stop-Process -Force
cmd /c "docker-compose -f `"$composeFile`" down -v --remove-orphans >nul 2>&1"

# --- Сборка ---
Write-Host "[+] Building..."
& cmake -B "$projectRoot\build" -S "$projectRoot" -DCMAKE_BUILD_TYPE=Debug
& cmake --build "$projectRoot\build" --config Debug --parallel

# --- Проверка наличия файлов ---
if (-not (Test-Path $serverExe)) { throw "❌ server.exe NOT FOUND at $serverExe" }
if (-not (Test-Path $clientExe)) { throw "❌ client.exe NOT FOUND at $clientExe" }

# --- Запуск БД ---
Write-Host "[+] Starting PostgreSQL..."
& docker-compose -f $composeFile up -d
Start-Sleep -Seconds 3

# --- Запуск приложений ---
Write-Host "[+] Launching server..."
Start-Process -FilePath $serverExe -WindowStyle Normal
Start-Sleep -Milliseconds 600

Write-Host "[+] Launching client..."
Start-Process -FilePath $clientExe -WindowStyle Normal

# --- Ожидание ---
Write-Host "`n[+] Running. Press ANY KEY to stop..."
$null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")

# --- Финальная очистка ---
Write-Host "`n[!] Shutting down..."
Get-Process server -ErrorAction SilentlyContinue | Stop-Process -Force
Get-Process client -ErrorAction SilentlyContinue | Stop-Process -Force
cmd /c "docker-compose -f `"$composeFile`" down -v --remove-orphans >nul 2>&1"
Write-Host "[✓] Done."