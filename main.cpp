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

#include "mbed.h"
#include "TCPSocket.h"

#include "stm32l475e_iot01_magneto.h"
#include "stm32l475e_iot01_gyro.h"
#include "stm32l475e_iot01_accelero.h"
#include <cstdio>
#include <cstring>
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
    char* sbuffer = "hello";
    nsapi_size_t size = strlen(sbuffer);
    response = 0;
    int count=0;

    int16_t pDataXYZ[3] = {0};
    float pGyroDataXYZ[3] = {0};

    BSP_ACCELERO_Init();
    
    while(1)
    {
        printf("\nNew loop, LED1 should blink during sensor read\n");

        ThisThread::sleep_for(1000);

        BSP_ACCELERO_AccGetXYZ(pDataXYZ);
        printf("\nACCELERO_X = %d\n", pDataXYZ[0]);
        printf("ACCELERO_Y = %d\n", pDataXYZ[1]);
        printf("ACCELERO_Z = %d\n", pDataXYZ[2]);


        // cout << "0" << endl;
        ThisThread::sleep_for(1000);

        cout << "1" << endl;
        sprintf(sbuffer, "%d", pDataXYZ[0]); 
        // cout << "2" << endl;
        size = strlen(sbuffer);
        // cout << "3" << endl;
        printf("%s", sbuffer);

        // float x = pDataXYZ[0], y = pDataXYZ[1], z = pDataXYZ[2];
        // int len = sprintf(sbuffer,"{\"x\":%f,\"y\":%f,\"z\":%f,\"s\":%d}",(float)((int)(x*10000))/10000, (float)((int)(y*10000))/10000, (float)((int)(z*10000))/10000, sample_num);


        ++count;
        response = socket.send(sbuffer, size);
        if (response < 0) {
            printf("Error sending data: %d\n", response);
            socket.close();
            return;
        }
        //if (count == 100) break;
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

    // int16_t pDataXYZ[3] = {0};
    // float pGyroDataXYZ[3] = {0};

    // BSP_ACCELERO_Init();

    // while(1)
    // {
    //     printf("\nNew loop, LED1 should blink during sensor read\n");

    //     ThisThread::sleep_for(1000);

    //     BSP_ACCELERO_AccGetXYZ(pDataXYZ);
    //     printf("\nACCELERO_X = %d\n", pDataXYZ[0]);
    //     printf("ACCELERO_Y = %d\n", pDataXYZ[1]);
    //     printf("ACCELERO_Z = %d\n", pDataXYZ[2]);

    //     ThisThread::sleep_for(1000);
    // }
}
