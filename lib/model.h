#pragma once
#include "interface.h"
#include "rainfall.h"

typedef struct 
{

    rainFall::model_t rainMenge;
    interface::model_t interface;

}model_t;

model_t model;

interface::model_t *interface_model = &model.interface;