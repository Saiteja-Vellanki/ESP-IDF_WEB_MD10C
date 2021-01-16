
Brief Introdction
control servo motor at desired angle using http server either wifi/ethernet
connect servo motors : to this pins 
On esp32 board :----------------------> GIPO 32 (D32)
                                     -> GPIO 33 (D33)
                                     -> GPIO 14 (D14)
                                     -> GPIO 12 (D12)
                                     -> GPIO 4   (D4)
                                     -> GPIO 16  (RX2)
defined macro definitions for set maximum angle for the motors ..
user can easily configure the value ..\
eg: #define MOTOR_GRIPPER_MAX_ANGLE      180 
if you want to change the angle for the gripper simply replace 180 with your desired angle..
if you change the angle at above you have to change the slider range , can find on html code, please find the below code snippet..
<input type=\"range\" min=\"0\" max=\"180\" class=\"slider\" change 180 to your desired angle....
#################Project Configuration###########
set up the ESP-IDF environment toolchain etc.
copy this entire directory to the path eg: esp/esp-idf/components/
open the directory which you have copied .
Type make menuconfig 
select the phy you need mean WIFI/ETHERNET and flash the code using command ** make flash monitor ** 
IP address will be generated ,Type the IP address on your browser..
Added ngrok function , to work this server globally mean not in the local n/w have to setup ngrok in your system and need to configure..

***********works with out internet connection***********(local n/w))
  

ETHERNET CONNECTIONS :::::

ESP32 BOARD :      ETHMODULE :

GPIO21:              TX-EN,
GPIO19:              TX0, 
GPIO22:              TX1,
GPIO25:              RX0,
GPIO26:              RX1,
GPIO27:              CRS_DV,
3.3V  :              VCC,
GND   :             GND.

