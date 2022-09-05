#include <Arduino.h>
#include "task3.h"
#include "events.h"

static bool compareKeys(BUTTONS *pSecret, BUTTONS *pKey)
{
    bool correct = true;
    for (uint8_t i = 0; i < 5; i++)
    {
        if (pSecret[i] != pKey[i])
        {
            correct = false;
            break;
        }
    }

    return correct;
}

void task3()
{

    enum class TaskStates
    {
        INIT,
        SLOW,
        MEDIUM,
        FAST,
        PERM_ON,
        PERM_OFF,
        WAIT_ON,
        WAIT_OFF,
    };
    static TaskStates taskState = TaskStates::INIT;

    static uint8_t led = 25; // Declaro la posición de la led

    static uint32_t lasTime;
    static constexpr uint32_t SLOW_TIME = 500;   // Cantidad de tiempo que se demora
    static constexpr uint32_t MEDIUM_TIME = 250; // Cantidad de tiempo que se demora
    static constexpr uint32_t FAST_TIME = 125;   // Cantidad de tiempo que se demora

    static bool ledStatus = false;
    static bool if_off = false; // Variable para preguntar si la led esta apagada o si la led estaba prendida

    static BUTTONS secret[5] = {BUTTONS::ONE_BTN, BUTTONS::ONE_BTN,
                                BUTTONS::TWO_BTN, BUTTONS::TWO_BTN,
                                BUTTONS::ONE_BTN}; // Codigo para salir del modo rapido

    static BUTTONS backkey[5] = {BUTTONS::NONE};

    static uint8_t keyCounter;

    switch (taskState)
    {
    case TaskStates::INIT:
    {
        pinMode(led, OUTPUT);
        digitalWrite(led, LOW);

        taskState = TaskStates::SLOW;
        break;
    }

    case TaskStates::SLOW:
    {
        uint32_t currentTime = millis();
        if ((currentTime - lasTime) >= SLOW_TIME)
        {
            lasTime = currentTime;
            ledStatus = !ledStatus;
            digitalWrite(led, ledStatus);            
        }

        if (buttonEvt.trigger == true)
        {
            buttonEvt.trigger = false;
            if (buttonEvt.whichButton == BUTTONS::ONE_BTN)
            {
                taskState = TaskStates::WAIT_OFF;
            }
            else if (buttonEvt.whichButton == BUTTONS::TWO_BTN)
            {
                taskState = TaskStates::MEDIUM;
            }
        }
        break;
    }

    case TaskStates::WAIT_OFF:
    {
        uint32_t currentTime = millis();
        if ((currentTime - lasTime) >= SLOW_TIME)
        {
            ledStatus = false;
            digitalWrite(led, ledStatus);
            taskState = TaskStates::PERM_OFF;
        }
    }
    case TaskStates::PERM_OFF:
    {         
        if (buttonEvt.trigger == true)
        {
            buttonEvt.trigger = false;
            if (buttonEvt.whichButton == BUTTONS::ONE_BTN)
            {
                taskState = TaskStates::SLOW;
            }
            else if (buttonEvt.whichButton == BUTTONS::TWO_BTN)
            {
                if_off = true;
                taskState = TaskStates::FAST;
            }
        }
        break;
    }

    case TaskStates::MEDIUM:
    {
        uint32_t currentTime = millis();
        if ((currentTime - lasTime) >= MEDIUM_TIME)
        {
            lasTime = currentTime;
            digitalWrite(led, ledStatus);
            ledStatus = !ledStatus;
        }
        if (buttonEvt.trigger == true)
        {
            buttonEvt.trigger = false;
            if (buttonEvt.whichButton == BUTTONS::ONE_BTN)
            {
                taskState = TaskStates::WAIT_ON;
                    
            }
            else if (buttonEvt.whichButton == BUTTONS::TWO_BTN)
            {
                taskState = TaskStates::SLOW;
            }
        }  
        break;
    }

    case TaskStates::WAIT_ON:
    {
        uint32_t currentTime = millis();
         if ((currentTime - lasTime) >= MEDIUM_TIME)
         {
            ledStatus = true;
            digitalWrite(led, ledStatus);
            taskState = TaskStates::PERM_ON;
         }
    }

    case TaskStates::PERM_ON:
    {                
        if (buttonEvt.trigger == true)
        {
            buttonEvt.trigger = false;
            if (buttonEvt.whichButton == BUTTONS::ONE_BTN)
            {
                taskState = TaskStates::MEDIUM;
            }
            else if (buttonEvt.whichButton == BUTTONS::TWO_BTN)
            {
                if_off = false;
                taskState = TaskStates::FAST;
            }
        }
        break;
    }
    case TaskStates::FAST:
    {
        uint32_t currentTime = millis();
        if ((currentTime - lasTime) >= FAST_TIME)
        {
            lasTime = currentTime;
            
            ledStatus = !ledStatus;
            digitalWrite(led, ledStatus);
        }

        if (buttonEvt.trigger == true)
        {
            buttonEvt.trigger = false;
            backkey[keyCounter] = buttonEvt.whichButton;
            keyCounter++;
            if (keyCounter == 5)
            {
                keyCounter = 0;
                if (compareKeys(secret, backkey) == true)
                {
                    Serial.print("Regresar al modo anterior\n");

                    if (if_off == true)
                    {
                        taskState = TaskStates::PERM_OFF; // Si estaba apagado, regresa a estar apagado
                    }
                    else
                    {
                        taskState = TaskStates::PERM_ON; // Si estaba prendido, regresa a estar prendido
                    }
                }
                else
                {
                    Serial.print("Lo lamento, te equivocaste.\n");
                }
            }
        }
        break;
    }
    default:
    {
        break;
    }
    }
}