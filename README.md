# Aries_Solutions
committed to Aries
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
comes to esp32 in the server start function uncomment the the below snippet..
    httpd_config_t config = HTTPD_DEFAULT_CONFIG_NGROK();
    and comment the below snippet
   // httpd_config_t config = HTTPD_DEFAULT_CONFIG();

     
static httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    //httpd_config_t config = HTTPD_DEFAULT_CONFIG_NGROK();
    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &hello);
        httpd_register_uri_handler(server, &echo);
        httpd_register_uri_handler(server, &ctrl);
        return server;
    }

