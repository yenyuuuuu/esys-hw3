/* WiFi Example
 * Copyright (c) 2018 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ThisThread.h"
#include "mbed.h"
#include "TCPSocket.h"

#include "stm32l475e_iot01_magneto.h"
#include "stm32l475e_iot01_gyro.h"
#include "stm32l475e_iot01_accelero.h"
#include <stdio.h>
#include <string.h>
#include <iostream>


#define WIFI_IDW0XX1    2

#if (defined(TARGET_DISCO_L475VG_IOT01A) || defined(TARGET_DISCO_F413ZH))
#include "ISM43362Interface.h"
ISM43362Interface wifi(false);

#else // External WiFi modules

#if MBED_CONF_APP_WIFI_SHIELD == WIFI_IDW0XX1
#include "SpwfSAInterface.h"
SpwfSAInterface wifi(MBED_CONF_APP_WIFI_TX, MBED_CONF_APP_WIFI_RX);
#endif // MBED_CONF_APP_WIFI_SHIELD == WIFI_IDW0XX1

#endif


const char *sec2str(nsapi_security_t sec)
{
    switch (sec) {
        case NSAPI_SECURITY_NONE:
            return "None";
        case NSAPI_SECURITY_WEP:
            return "WEP";
        case NSAPI_SECURITY_WPA:
            return "WPA";
        case NSAPI_SECURITY_WPA2:
            return "WPA2";
        case NSAPI_SECURITY_WPA_WPA2:
            return "WPA/WPA2";
        case NSAPI_SECURITY_UNKNOWN:
        default:
            return "Unknown";
    }
}

void http_demo(NetworkInterface *net)
{
    TCPSocket socket;
    nsapi_error_t response;

    // Show the network address
    SocketAddress a;
    net->get_ip_address(&a);
    printf("IP address: %s\n", a.get_ip_address() ? a.get_ip_address() : "None");
    printf("Sending request to 192.168.50.254...\n");
    // Open a socket on the network interface, and create a TCP connection to
    //www.arm.com
    socket.open(net);
    net->gethostbyname("192.168.50.254", &a);
    a.set_port(3000);
    response = socket.connect(a);
    if(0 != response) {
        printf("Error connecting: %d\n", response);
        socket.close();
        return;
    }

    // Send a simple http request
    char sbuffer[100] = "hello";
    nsapi_size_t size = strlen(sbuffer);
    response = 0;
    int count=0; //, sample_num=0

    int16_t pDataXYZ[3] = {0};
    float pGyroDataXYZ[3] = {0};

    BSP_ACCELERO_Init();
    // BSP_MAGNETO_Init();
    // BSP_GYRO_Init();

    
    while(1)
    {

        BSP_ACCELERO_AccGetXYZ(pDataXYZ);
        // printf("\nACCELERO_X = %d\n", pDataXYZ[0]);
        // printf("ACCELERO_Y = %d\n", pDataXYZ[1]);
        // printf("ACCELERO_Z = %d\n", pDataXYZ[2]);

        // BSP_GYRO_GetXYZ(pGyroDataXYZ);
        // printf("\nGYRO_X = %.2f\n", pGyroDataXYZ[0]);
        // printf("GYRO_Y = %.2f\n", pGyroDataXYZ[1]);
        // printf("GYRO_Z = %.2f\n", pGyroDataXYZ[2]);


        // BSP_MAGNETO_GetXYZ(pDataXYZ);
        // printf("\nMAGNETO_X = %d\n", pDataXYZ[0]);
        // printf("MAGNETO_Y = %d\n", pDataXYZ[1]);
        // printf("MAGNETO_Z = %d\n", pDataXYZ[2]);


        ThisThread::sleep_for(200);

        float ax = pDataXYZ[0], ay = pDataXYZ[1], az = pDataXYZ[2]/100;
        int alen = sprintf(sbuffer,"{\"x\":%.2f,\"y\":%.2f,\"z\":%.2f}",(float)((int)(ax*10000))/10000, (float)((int)(ay*10000))/10000, (float)((int)(az*10000))/10000);
        // printf("%s", sbuffer);
        // response = socket.send(sbuffer, alen);

        // float gx = pGyroDataXYZ[0], gy = pGyroDataXYZ[1] , gz = pGyroDataXYZ[2];
        // int blen = sprintf(sbuffer,"{\"x\":%.2f,\"y\":%.2f,\"z\":%.2f}",(float)((int)(gx*10000))/10000, (float)((int)(gy*10000))/10000, (float)((int)(gz*10000))/10000);
        printf("%s, count= %d \n", sbuffer, count);
        response = socket.send(sbuffer, alen);


        ++count;
        ThisThread::sleep_for(500);
        // if (response < 0) {
        //     printf("Error sending data: %d\n", response);
        //     socket.close();
        //     return;
        // }
        if (count == 200) break;
    }

    // Recieve a simple http response and print out the response line
    // char rbuffer[64];
    // response = socket.recv(rbuffer, sizeof rbuffer);
    // if (response < 0) {
    //     printf("Error receiving data: %d\n", response);
    // } else {
    //     printf("recv %d [%.*s]\n", response, strstr(rbuffer, "\r\n")-rbuffer, rbuffer);
    // }

    // Close the socket to return its memory and bring down the network interface
    socket.close();
}

int main()
{

    printf("\nConnecting to %s...\n", MBED_CONF_APP_WIFI_SSID);
    int ret = wifi.connect(MBED_CONF_APP_WIFI_SSID, MBED_CONF_APP_WIFI_PASSWORD, NSAPI_SECURITY_WPA_WPA2);
    if (ret != 0) {
        printf("\nConnection error\n");
        return -1;
    }

    printf("Success\n\n");
    printf("MAC: %s\n", wifi.get_mac_address());
    printf("IP: %s\n", wifi.get_ip_address());
    printf("Netmask: %s\n", wifi.get_netmask());
    printf("Gateway: %s\n", wifi.get_gateway());
    printf("RSSI: %d\n\n", wifi.get_rssi());


    http_demo(&wifi);

    wifi.disconnect();

    printf("\nDone\n");
}
