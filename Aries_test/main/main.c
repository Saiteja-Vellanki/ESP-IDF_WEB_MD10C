/* Simple HTTP Server Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include "nvs_flash.h"
//#include "esp_netif.h"
#include "tcpip_adapter.h"
#include "esp_eth.h"
#include "protocol_examples_common.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_attr.h"
#include "driver/mcpwm.h"
#include "soc/mcpwm_periph.h"
#include <esp_http_server.h>

/* A simple example that demonstrates how to create GET and POST
 * handlers for the web server.
 */

//macro definitions 

#define MOTOR_MIN_PULSE_WIDTH 1000        //microseconds
#define MOTOR_MAX_PULSE_WIDTH 2000

//macro definitions for five motors angles
//Can change max angle ranges depending range of the motors

#define MOTOR_GRIPPER_MAX_ANGLE      360
#define MOTOR_WRIST_JOINT_MAX_ANGLE  180 
#define MOTOR_ELBOW_JOINT_MAX_ANGLE  180 
#define MOTOR_SHOULDER_MAX_ANGLE     180 
#define MOTOR_BASE_MAX_ANGLE         360 

//macro definitions for PINS to control the PWM signal of the motor

 #define MOTOR_GRIPPER_PWM_CONTROL_PIN          32 
 #define MOTOR_WRIST_JOINT_PWM_CONTROL_PIN      33 
 #define MOTOR_ELBOW_JOINT_PWM_CONTROL_PIN      14
 #define MOTOR_SHOULDER_PWM_CONTROL_PIN         12
 #define MOTOR_BASE_PWM_CONTROL_PIN             4
static const char *TAG = "Aries Robotic ARM";


static uint32_t motor_per_degree_cal_grip(uint32_t grip_rotation_deg)
{
    uint32_t grip_pulse_width_cal = 0;
    grip_pulse_width_cal = (MOTOR_MIN_PULSE_WIDTH + (((MOTOR_MAX_PULSE_WIDTH - MOTOR_MIN_PULSE_WIDTH) * (grip_rotation_deg)) / (MOTOR_GRIPPER_MAX_ANGLE)));
    return grip_pulse_width_cal;
}

static uint32_t motor_per_degree_cal_wrist(uint32_t wrist_rotation_deg)
{
    uint32_t wrist_pulse_width_cal = 0;
    wrist_pulse_width_cal = (MOTOR_MIN_PULSE_WIDTH + (((MOTOR_MAX_PULSE_WIDTH - MOTOR_MIN_PULSE_WIDTH) * (wrist_rotation_deg)) / (MOTOR_WRIST_JOINT_MAX_ANGLE)));
    return wrist_pulse_width_cal;
}

static uint32_t motor_per_degree_cal_elbow(uint32_t elbow_rotation_deg)
{
    uint32_t elbow_pulse_width_cal = 0;
    elbow_pulse_width_cal = (MOTOR_MIN_PULSE_WIDTH + (((MOTOR_MAX_PULSE_WIDTH - MOTOR_MIN_PULSE_WIDTH) * (elbow_rotation_deg)) / (MOTOR_ELBOW_JOINT_MAX_ANGLE)));
    return elbow_pulse_width_cal;
}

static uint32_t motor_per_degree_cal_shoulder(uint32_t shoulder_rotation_deg)
{
    uint32_t shoulder_pulse_width_cal = 0;
    shoulder_pulse_width_cal = (MOTOR_MIN_PULSE_WIDTH + (((MOTOR_MAX_PULSE_WIDTH - MOTOR_MIN_PULSE_WIDTH) * (shoulder_rotation_deg)) / (MOTOR_SHOULDER_MAX_ANGLE)));
    return shoulder_pulse_width_cal;
}

static uint32_t motor_per_degree_cal_base(uint32_t base_rotation_deg)
{
    uint32_t base_pulse_width_cal = 0;
    base_pulse_width_cal = (MOTOR_MIN_PULSE_WIDTH + (((MOTOR_MAX_PULSE_WIDTH - MOTOR_MIN_PULSE_WIDTH) * (base_rotation_deg)) / (MOTOR_BASE_MAX_ANGLE)));
    return base_pulse_width_cal;
}




/* An HTTP GET handler */
static esp_err_t hello_get_handler(httpd_req_t *req)
{
    char*  buf;
    size_t buf_len;

    /* Get header value string length and allocate memory for length + 1,
     * extra byte for null termination */
    buf_len = httpd_req_get_hdr_value_len(req, "Host") + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        /* Copy null terminated value string into buffer */
        if (httpd_req_get_hdr_value_str(req, "Host", buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found header => Host: %s", buf);
        }
        free(buf);
    }

    buf_len = httpd_req_get_hdr_value_len(req, "Test-Header-2") + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        if (httpd_req_get_hdr_value_str(req, "Test-Header-2", buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found header => Test-Header-2: %s", buf);
        }
        free(buf);
    }

    buf_len = httpd_req_get_hdr_value_len(req, "Test-Header-1") + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        if (httpd_req_get_hdr_value_str(req, "Test-Header-1", buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found header => Test-Header-1: %s", buf);
        }
        free(buf);
    }

    /* Read URL query string length and allocate memory for length + 1,
     * extra byte for null termination */
    buf_len = httpd_req_get_url_query_len(req) + 1;
    if (buf_len > 1) {
        buf = malloc(buf_len);
        if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
            //ESP_LOGI(TAG, "Found URL query => %s", buf);
            char param[32];
            /* Get value of expected key from query string */
             if (httpd_query_key_value(buf, "grip", param, sizeof(param)) == ESP_OK) {
                ESP_LOGI(TAG, "Found URL query parameter => GRIPPER=%s", param);
                int gripangle=0;
                int gripcount = atoi(param);
                gripangle = motor_per_degree_cal_grip(gripcount);
                printf("gripper value %d\n",gripcount);
                mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, gripangle);
                
            }
            if (httpd_query_key_value(buf, "wrist", param, sizeof(param)) == ESP_OK) {
                ESP_LOGI(TAG, "Found URL query parameter => WRIST_JOINT=%s", param);
                int wristangle=0;
                int wristcount = atoi(param);
                wristangle = motor_per_degree_cal_wrist(wristcount);
                printf("wrist value %d\n",wristcount);
                mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, wristangle);
            }
            if (httpd_query_key_value(buf, "elbow", param, sizeof(param)) == ESP_OK) {
                ESP_LOGI(TAG, "Found URL query parameter => ELBOW_JOINT=%s", param);
                int elbowangle=0;
                int elbowcount = atoi(param);
                elbowangle = motor_per_degree_cal_elbow(elbowcount);
                printf("elbow value %d\n",elbowcount);
                mcpwm_set_duty_in_us(MCPWM_UNIT_1, MCPWM_TIMER_0, MCPWM_OPR_A, elbowangle);
            }  
            if (httpd_query_key_value(buf, "shoulder", param, sizeof(param)) == ESP_OK) {
   
                ESP_LOGI(TAG, "Found URL query parameter => SHOULDER=%s", param);
                int shoulderangle=0;
                int shouldercount = atoi(param);
                shoulderangle = motor_per_degree_cal_shoulder(shouldercount);
                printf("shoulder value %d\n",shouldercount);
                mcpwm_set_duty_in_us(MCPWM_UNIT_1, MCPWM_TIMER_0, MCPWM_OPR_B, shoulderangle);
            }

            if (httpd_query_key_value(buf, "base", param, sizeof(param)) == ESP_OK) {
                ESP_LOGI(TAG, "Found URL query parameter => BASE=%s", param);
                int baseangle=0;
                int basecount = atoi(param);
                baseangle = motor_per_degree_cal_base(basecount);
                printf("base value %d\n",basecount);
                mcpwm_set_duty_in_us(MCPWM_UNIT_1, MCPWM_TIMER_0, MCPWM_OPR_A, baseangle);
            }
             

        }
        free(buf);
    }

    /* Set some custom headers */
    httpd_resp_set_hdr(req, "Custom-Header-1", "Custom-Value-1");
    httpd_resp_set_hdr(req, "Custom-Header-2", "Custom-Value-2");

    /* Send response with custom headers and body set as the
     * string passed in user context*/
    const char* resp_str = (const char*) req->user_ctx;
    httpd_resp_send(req, resp_str, strlen(resp_str));

    /* After sending the HTTP response the old HTTP request
     * headers are lost. Check if HTTP request headers can be read now. */
    if (httpd_req_get_hdr_value_len(req, "Host") == 0) {
        ESP_LOGI(TAG, "Request headers lost");
    }
    return ESP_OK;
}

static const httpd_uri_t hello = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = hello_get_handler,
    .user_ctx  = "<!DOCTYPE html>\r\n<title> Aries <\/title><html>\r\n  <head><fieldset ><center><h1 style=\"color:green;\">Aries Solutions<\/h1>\r\n  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\r\n  <link rel=\"icon\" href=\"data:,\">\r\n  <style>\r\n    body {\r\n      text-align: center;\r\n      font-family: \"Trebuchet MS\", Arial;\r\n      margin-left:auto;\r\n      margin-right:auto;\r\n    }\r\n    .slider {\r\n      width: 300px;\r\n    }\r\n  <\/style>\r\n  <script src=\"https:\/\/ajax.googleapis.com\/ajax\/libs\/jquery\/3.3.1\/jquery.min.js\"><\/script>\r\n<\/head>\r\n<body>\r\n  <h1 style=\"color:red;\">Robotic ARM Control<\/h1>\r\n  <p style=\"color:green;\">Position: <span id=\"GRIPP\"><\/span><\/p>\r\n   <label style=\"color:green;\" for=\"GRIPPER\">Gripper &emsp;\r\n  <input type=\"range\" min=\"0\" max=\"360\" class=\"slider\" id=\"GRIPPER\" oninput=\"grip(this.value)\"\/><br\/>\r\n  <p>Position: <span id=\"WRIST\"><\/span><\/p>\r\n  <label style=\"color:green;\" for=\"WRIST JOINT\">WRIST JOINT &emsp;\r\n  <input type=\"range\" min=\"0\" max=\"180\" class=\"slider\" id=\"WRIST JOINT\" oninput=\"wrist(this.value)\"\/><br\/>\r\n  <p>Position: <span id=\"ELBOW\"><\/span><\/p>\r\n  <label style=\"color:green;\" for=\"ELBOW JOINT\">ELBOW JOINT &emsp;\r\n  <input type=\"range\" min=\"0\" max=\"180\" class=\"slider\" id=\"ELBOW JOINT\" oninput=\"elbow(this.value)\"\/><br\/>\r\n  <p>Position: <span id=\"SHOULDE\"><\/span><\/p>\r\n  <label style=\"color:green;\" for=\"SHOULDER\">SHOULDER  &emsp;\r\n  <input type=\"range\" min=\"0\" max=\"180\" class=\"slider\" id=\"SHOULDER\" oninput=\"shoulder(this.value)\"\/><br\/>\r\n  <p>Position: <span id=\"BAS\"><\/span><\/p>\r\n  <label style=\"color:green;\" for=\"BASE\">BASE  &emsp;\r\n  <input type=\"range\" min=\"0\" max=\"360\" class=\"slider\" id=\"BASE\" oninput=\"base(this.value)\"\/><br\/>\r\n  <script>\r\n   \r\n    var slider = document.getElementById(\"GRIPP\");\r\n    var servoP = document.getElementById(\"GRIPPER\");\r\n    servoP.innerHTML = slider.value;\r\n    slider.oninput = function() {\r\n      slider.value = this.value;\r\n      servoP.innerHTML = this.value;\r\n    }\r\n    function grip(pos) {\r\n      $.get(\"\/?grip=\" + pos + \"&\");\r\n      {Connection: close};\r\n    }\r\n\r\n    var slider = document.getElementById(\"WRIST\");\r\n    var servoP = document.getElementById(\"WRIST JOINT\") ;\r\n    servoP.innerHTML = slider.value;\r\n    slider.oninput = function() {\r\n      slider.value = this.value;\r\n      servoP.innerHTML = this.value;\r\n    }\r\n    function wrist(pos) {\r\n      $.get(\"\/?wrist=\" + pos + \"&\");\r\n      {Connection: close};\r\n    }\r\n    \r\n    var slider = document.getElementById(\"ELBOW\");\r\n    var servoP = document.getElementById(\"ELBOW JOINT\");\r\n    servoP3.innerHTML = slider.value;\r\n    slider.oninput = function() {\r\n      slider.value = this.value;\r\n      servoP.innerHTML = this.value;\r\n    }\r\n       function elbow(pos) {\r\n      $.get(\"\/?elbow=\" + pos + \"&\");\r\n      {Connection: close};\r\n    }\r\n   \r\n    var slider = document.getElementById(\"SHOULDE\");\r\n    var servoP = document.getElementById(\"SHOULDER\");\r\n    servoP.innerHTML = slider.value;\r\n    slider.oninput = function() {\r\n      slider.value = this.value;\r\n      servoP.innerHTML = this.value;\r\n    }\r\n     function shoulder(pos) {\r\n      $.get(\"\/?shoulder=\" + pos + \"&\");\r\n      {Connection: close};\r\n    }\r\n    \r\n     var slider = document.getElementById(\"BAS\");\r\n    var servoP = document.getElementById(\"BASE\");\r\n    servoP.innerHTML = slider.value;\r\n    slider.oninput = function() {\r\n      slider.value = this.value;\r\n      servoP.innerHTML = this.value;\r\n    }\r\n    function base(pos) {\r\n      $.get(\"\/?base=\" + pos + \"&\");\r\n      {Connection: close};\r\n    }\r\n  <\/script>\r\n<\/body>\r\n<\/html>"
};

/* An HTTP POST handler */
static esp_err_t echo_post_handler(httpd_req_t *req)
{
    char buf[100];
    int ret, remaining = req->content_len;

    while (remaining > 0) {
        /* Read the data for the request */
        if ((ret = httpd_req_recv(req, buf,
                        MIN(remaining, sizeof(buf)))) <= 0) {
            if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
                /* Retry receiving if timeout occurred */
                continue;
            }
            return ESP_FAIL;
        }

        /* Send back the same data */
        httpd_resp_send_chunk(req, buf, ret);
        remaining -= ret;

        /* Log data received */
        ESP_LOGI(TAG, "=========== RECEIVED DATA ==========");
        ESP_LOGI(TAG, "%.*s", ret, buf);
        ESP_LOGI(TAG, "====================================");
    }

    // End response
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

static const httpd_uri_t echo = {
    .uri       = "/",
    .method    = HTTP_POST,
    .handler   = echo_post_handler,
    .user_ctx  = NULL
};

/* This handler allows the custom error handling functionality to be
 * tested from client side. For that, when a PUT request 0 is sent to
 * URI /ctrl, the /hello and /echo URIs are unregistered and following
 * custom error handler http_404_error_handler() is registered.
 * Afterwards, when /hello or /echo is requested, this custom error
 * handler is invoked which, after sending an error message to client,
 * either closes the underlying socket (when requested URI is /echo)
 * or keeps it open (when requested URI is /hello). This allows the
 * client to infer if the custom error handler is functioning as expected
 * by observing the socket state.
 */
esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err)
{
    if (strcmp("/", req->uri) == 0) {
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "/ URI is not available");
        /* Return ESP_OK to keep underlying socket open */
        return ESP_OK;
    } else if (strcmp("/", req->uri) == 0) {
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "/ URI is not available");
        /* Return ESP_FAIL to close underlying socket */
        return ESP_FAIL;
    }
    /* For any other URI send 404 and close socket */
    httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Some 404 error message");
    return ESP_FAIL;
}

/* An HTTP PUT handler. This demonstrates realtime
 * registration and deregistration of URI handlers
 */
static esp_err_t ctrl_put_handler(httpd_req_t *req)
{
    char buf;
    int ret;

    if ((ret = httpd_req_recv(req, &buf, 1)) <= 0) {
        if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
            httpd_resp_send_408(req);
        }
        return ESP_FAIL;
    }

    if (buf == '0') {
        /* URI handlers can be unregistered using the uri string */
        ESP_LOGI(TAG, "Unregistering  URIs");
        httpd_unregister_uri(req->handle, "/");
        httpd_unregister_uri(req->handle, "/");
        /* Register the custom error handler */
        httpd_register_err_handler(req->handle, HTTPD_404_NOT_FOUND, http_404_error_handler);
    }
    else {
        ESP_LOGI(TAG, "Registering  URIs");
        httpd_register_uri_handler(req->handle, &hello);
        httpd_register_uri_handler(req->handle, &echo);
        /* Unregister custom error handler */
        httpd_register_err_handler(req->handle, HTTPD_404_NOT_FOUND, NULL);
    }

    /* Respond with empty body */
    httpd_resp_send(req, NULL, 0);
    return ESP_OK;
}

static const httpd_uri_t ctrl = {
    .uri       = "/ctrl",
    .method    = HTTP_PUT,
    .handler   = ctrl_put_handler,
    .user_ctx  = NULL
};

static httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

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

    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}

static void stop_webserver(httpd_handle_t server)
{
    // Stop the httpd server
    httpd_stop(server);
}

static void disconnect_handler(void* arg, esp_event_base_t event_base, 
                               int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server) {
        ESP_LOGI(TAG, "Stopping webserver");
        stop_webserver(*server);
        *server = NULL;
    }
}

static void connect_handler(void* arg, esp_event_base_t event_base, 
                            int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server == NULL) {
        ESP_LOGI(TAG, "Starting webserver");
        *server = start_webserver();
    }
}

static void servo_gpio_init(void)
{
    printf("Aries :::initializing GPIO pins for the motors:::\n");
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, MOTOR_GRIPPER_PWM_CONTROL_PIN);    //Set GPIO  as PWM0A/B, to which servo is connected
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0B, MOTOR_WRIST_JOINT_PWM_CONTROL_PIN); 
    mcpwm_gpio_init(MCPWM_UNIT_1, MCPWM1A, MOTOR_ELBOW_JOINT_PWM_CONTROL_PIN); 
    mcpwm_gpio_init(MCPWM_UNIT_1, MCPWM1B, MOTOR_SHOULDER_PWM_CONTROL_PIN); 
    mcpwm_gpio_init(MCPWM_UNIT_1, MCPWM2A, MOTOR_BASE_PWM_CONTROL_PIN); 
   
}

static void pwm_init(void)
{
   printf("Aries :::Configuring Initial Parameters of pwm......\n");
    mcpwm_config_t pwm_config;
    pwm_config.frequency = 10;    //frequency = 10Hz, i.e. for every servo motor time period should be 20ms
    pwm_config.cmpr_a = 0;    //duty cycle of PWMxA = 0
    pwm_config.cmpr_b = 0;    //duty cycle of PWMxb = 0
    pwm_config.counter_mode = MCPWM_UP_COUNTER;
    pwm_config.duty_mode = MCPWM_DUTY_MODE_0;
    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);   
    mcpwm_init(MCPWM_UNIT_1, MCPWM_TIMER_0, &pwm_config); 
   // mcpwm_init(MCPWM_UNIT_1, MCPWM_TIMER_0, &pwm_config); 

}


void app_main(void)
{
    static httpd_handle_t server = NULL;

    ESP_ERROR_CHECK(nvs_flash_init());
   // ESP_ERROR_CHECK(esp_netif_init());
     tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    servo_gpio_init();
    pwm_init();
    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    ESP_ERROR_CHECK(example_connect());

    /* Register event handlers to stop the server when Wi-Fi or Ethernet is disconnected,
     * and re-start it upon connection.
     */
#ifdef CONFIG_EXAMPLE_CONNECT_WIFI
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &connect_handler, &server));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &disconnect_handler, &server));
#endif // CONFIG_EXAMPLE_CONNECT_WIFI
#ifdef CONFIG_EXAMPLE_CONNECT_ETHERNET
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &connect_handler, &server));
    ESP_ERROR_CHECK(esp_event_handler_register(ETH_EVENT, ETHERNET_EVENT_DISCONNECTED, &disconnect_handler, &server));
#endif // CONFIG_EXAMPLE_CONNECT_ETHERNET

    /* Start the server for the first time */
    server = start_webserver();
    
}
