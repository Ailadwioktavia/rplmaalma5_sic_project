@echo off
REM Cek apakah Mosquitto sudah terinstal
if not exist "C:\Program Files\mosquitto\mosquitto.exe" (
    echo Mosquitto tidak ditemukan. Pastikan sudah terinstal di C:\Program Files\mosquitto.
    exit /b
)

REM Mulai Mosquitto
echo Memulai Mosquitto...
start "" "C:\Program Files\mosquitto\mosquitto.exe" -v -c mosquitto.conf

REM Tunggu beberapa detik untuk memastikan Mosquitto berjalan
timeout /t 5

REM Memeriksa status Mosquitto
echo Memeriksa status Mosquitto...
tasklist | findstr "mosquitto.exe"
if %errorlevel%==0 (
    echo Mosquitto berjalan dengan baik!
) else (
    echo Gagal menjalankan Mosquitto.
)
