# Serial Display Adapter

Adaptador USB <-> RS-232 con display que muestra mensajes de la forma:

T1:18.10, H1:60.00, T2:18.90, H2:60.00, CS:70

que es la salida de https://github.com/cpantel/CoolerControl



Para compilar:

git clone ciaa firmware_v3

[program.mk]
PROGRAM_PATH = projects/SerialDisplayAdapter
PROGRAM_NAME = edu-ciaa-nxp

poner este proyecto en projects o hacer un symlink



Mas información en https://seguridad-agile.blogspot.com/2020/10/adaptador-usb-rs-232-con-display-usando.html

