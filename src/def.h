/*
    File name: def.h
    Date: 2024.11.14
    Author: Wilhelm Kuckelsberg
    Description: Definition of PINs and constants

    Usable PINs
    D1  GPIO5
    D2  GPIO4
    D3  GPIO0
    D5  GPIO14
    D6  GPIO12
    D7  GPIO13
    A0  ADC0
*/

#define DALLAS         0    // D3
#define POOL_PUMP      5    // D1
#define SPRINKLER_EAST 14    // D5 
#define SPRINKLER_WEST  12    // D6
#define POOL_FILL       4    // D2
#define RAINDROP_PIN  13    // D7

#define DEBOUNCE_TIME 200   // 200 milliseconds
#define TIME_OUT_1  10000   // 10 sec

#define DEBUG_MESSAGE_BUFFER_SIZE 200
#define GLOBAL_BUFFER_SIZE 100

#define PORT_FOR_GARDENSERVICE 4001
#define DEBOUNCE_TIME 200

#define TIMEOUT_PONT 31UL * 60UL * 1000UL   // ~ 30 Minuten + 1 minute waiting period
#define TIMEOUT_HEATPUMP 61UL * 60UL * 1000UL   // ~ 60 Minuten + 1 minute waiting period

#define TIMEOUT_HCL_PUMP 16UL * 1000UL  // ~ 15 Seconds + 1 second waiting period
#define TIMEOUT_NAOH_PUMP 16UL * 1000UL  // ~ 15 Seconds + 1 second waiting period
#define TIMEOUT_ALGIZID_PUMP 16UL * 1000UL  // ~ 15 Seconds + 1 second waiting period
