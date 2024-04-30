/*
 * Copyright (c) 2020 Arm Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "DigitalOut.h"
#include "mbed.h"
#include "uop_msb.h"
#include "EthernetInterface.h"
#include <chrono>
#include <cstdio>
#include <ctime>
#include <iostream>
#include <cstring>
#include <string.h>
#include <string>
#include <mutex>
using namespace uop_msb;
using namespace std;

// network variables and information
EthernetInterface net;

#define IPV4_HOST_ADDRESS "192.168.137.1"
#define TCP_SOCKET_PORT 8082
//Light sensor
AnalogIn ldr(AN_LDR_PIN);
//string to hold sampled data
string output;
//lock used in FIFO buffer
Mutex BufferLock;
//Push Buttons
Buttons button;

//Environmental Sensor
EnvSensor env;   

//Display
LCD_16X2_DISPLAY disp;

//LEDs
DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);
DigitalOut RedLed(LED_RED);

//function declaration and threads used for said functions
void ReadData();
void TCPConnection();
Thread t1, t2;

//loop count to keep track of what data should be sampled
int ReadCount = 0;
//storage for sampled data
float light, temp, pres;
//timer to adjust read loop timeouts
Timer t;
//storage for time out of sync
int OutOfSync;
//storage for amount of time to sleep
int SleepTime;


//template class for buffer
template<typename T, int length>
class Buffer {

private:
    //data included in the buffer class
    T Items[length];
    int writepointer;
    int readpointer;

public:
    Buffer () : writepointer(0), readpointer(0){};
    //read an item from the buffer
    T ReadItem()
    {
        string temp;
        //try lock
        if(BufferLock.trylock_for(5s) == true)
        {temp = Items[readpointer];
        //output data read and buffer pointer in console
        cout << "Read " + temp + " from " + to_string(readpointer) << endl;
        readpointer = readpointer + 1;
        if(readpointer == 20)
        {
            readpointer = 0;
        }
        BufferLock.unlock();
        }
        else{
            //show error in console
            temp = "error";
            cout << "Error Aquiring read lock" << endl;
            RedLed = 1;
        }
        return temp;
    }
    //write item to buffer
    void WriteItem ( T& item)
    {
        if(BufferLock.trylock_for(5s) == true)
        {
          Items[writepointer] = item;
        cout << "Written " + item + " to " + to_string(writepointer) << endl;
        writepointer = writepointer + 1;
        if(readpointer == writepointer)
        {
            readpointer = readpointer + 1;
        }
        if(writepointer == 20)
        {
            writepointer = 0;
        }
        BufferLock.unlock();  
        }
        else{
            cout << "Error Aquiring write lock" << endl;
            RedLed = 1;
        }
        
        
    }

};
Buffer<string, 20> OutputBuffer;

void ReadData() {

    

    auto readSensors = []() {
        //start timer for thread sleep time
        t.reset();
        t.start();
        
        //sample light
        light = 1.0f - ldr.read();

        //sample temperature if needed
        
        temp  = env.getTemperature();
        
        //sample pressure if needed
        
        pres  = env.getPressure();
        
        //increment readcount
        ReadCount = ReadCount + 1;

        
        
        //combine outputs into string
        output = to_string(light) + "%"+ to_string(pres) + "%" + to_string(temp);
        

        
        
        //stop timer
        t.stop();

        // Keeps system timing more accurate by sleeping for an amount of time that adjusts to meet the time required to gather the data.
        OutOfSync = OutOfSync + t.elapsed_time().count();
        cout << "Time out of sync " + to_string(OutOfSync) + " Microseconds"<< endl;
        if(OutOfSync > 1000)
        {
            SleepTime = 999;
            OutOfSync = OutOfSync - 1000;
        }
        else{
            SleepTime = 1000;
        }

    };

    

    while(true) {
        //write data to buffer and loop after a time
        ThisThread::sleep_for(chrono::milliseconds(SleepTime));
        readSensors();
        OutputBuffer.WriteItem(output);
        
    }

}

void TCPConnection() {

    cout << "------------------------------" << endl;
    net.connect();
    cout << "test" << endl;
    bool keepGoing = true;
    string test = "1";
    char rbuffer[56];
    
    
    do {
        // Show the network address
        SocketAddress a;
        net.get_ip_address(&a);
        
        printf("IP address: %s\n", a.get_ip_address());

        // Open a socket on the network interface, and create a TCP connection to the TCP server
        TCPSocket socket;
        socket.open(&net);

        

        //Manually set the address
        a.set_ip_address(IPV4_HOST_ADDRESS);

        //Set the TCP socket port
        a.set_port(TCP_SOCKET_PORT);
        
        //Connect to remote web server
        socket.connect(a);

        
        
        //char array to store data to send
        char sbuffer[89];
        
        
        

        int scount;
        //send data
        strcpy(sbuffer, OutputBuffer.ReadItem().c_str());
        scount = socket.send(sbuffer, sizeof sbuffer);
        
        printf("sent\r\n");

        int rcount;

        // Receieve response and display it
        while ((rcount = socket.recv(rbuffer, 56)) > 0) {
            rbuffer[rcount] = 0;
            printf("%s", rbuffer);
        }
        printf("\n");

        
        


        // Close the socket
        socket.close();

        //Loop delay of 1s
        ThisThread::sleep_for(1s);

    } while (keepGoing);

    net.disconnect();
}

int main() {
    //start threads
    t1.start(TCPConnection);
    t2.start(ReadData);
    
    return 0;
}

    

