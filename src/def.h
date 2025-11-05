/*
    File name: def.h
    Date: 2024.11.14
    Author: Wilhelm Kuckelsberg
    Description: Definition of PINs

    Usable PINs
    D1  GPIO5
    D2  GPIO4
    D3  GPIO0
    D5  GPIO14
    D6  GPIO12
    D7  GPIO13
    A0  ADC0
*/

#define DEBUG_MESSAGE_BUFFER_SIZE 100
#define GLOBAL_BUFFER_SIZE 100

#define POOL_PUMP        5    // D1

#define DALLAS           0    // D3
#define VALVE_TERRACE 14   // D5 
#define VALVE_GARDEN  12   // D6
#define VALVE_RINSE  4    // D2
#define TRIGGER_PIN      13   // D7

#define DEBOUNCE_TIME 200   // 200 millisec
#define TIME_OUT_1 10000    //10sec