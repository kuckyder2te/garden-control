#pragma once
#include "bmp180.h"
#include "dht22.h"
#include "interface.h"

typedef struct 
{
    bmp180::model_t pressure;
    dht22::model_t climate;
    interface::model_t interface;
}model_t;

model_t MODEL;

interface::model_t *interface_model = &MODEL.interface;