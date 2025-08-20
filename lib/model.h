#pragma once
#include "interface.h"
#include "rainfall.h"

typedef struct 
{
    rainFall::model_t rainMenge;
    interface::model_t interface;
    volatile uint16_t count;
}model_t;

model_t model;

interface::model_t *interface_model = &model.interface;