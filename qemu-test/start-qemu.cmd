@echo off
cd /d "%~dp0"
if not exist nova-uefi.img powershell -NoProfile -Command "Expand-Archive -Force nova-uefi.zip ."
copy /y edk2-x86_64.fd edk2-vars.fd >nul
qemu-system-x86_64 -machine q35 -m 256M -smp 2 -drive if=pflash,format=raw,file=edk2-vars.fd -drive format=raw,file=nova-uefi.img,if=ide -serial file:serial.log
