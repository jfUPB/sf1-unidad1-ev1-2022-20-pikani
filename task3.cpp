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

    static uint8_t led = 25; 

    static uint32_t lasTime;
    static constexpr uint32_t SLOW_TIME = 500;   
    static constexpr uint32_t MEDIUM_TIME = 250; 
    static constexpr uint32_t FAST_TIME = 125;   

    static bool ledStatus = false;
    static bool if_off = false; 

    static BUTTONS secret[5] = {BUTTONS::ONE_BTN, BUTTONS::ONE_BTN,
                                BUTTONS::TWO_BTN, BUTTONS::TWO_BTN,
                                BUTTONS::ONE_BTN}; 

    static BUTTONS backkey[5] = {BUTTONS::NONE};

    static uint8_t keyCounter;

    switch (taskState)
    {
    case TaskStates::INIT:
    {
        ledStatus = LOW;
        pinMode(led, OUTPUT);
        digitalWrite(led, ledStatus);
        lasTime = 0;
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
                ledStatus = HIGH;
                pinMode(led, OUTPUT);
                digitalWrite(led, ledStatus);
                taskState = TaskStates::WAIT_OFF;
            }
            else if (buttonEvt.whichButton == BUTTONS::TWO_BTN)
            {
                ledStatus = LOW;
                pinMode(led, OUTPUT);
                digitalWrite(led, ledStatus);
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
            pinMode(led, OUTPUT);
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
                ledStatus = LOW;
                pinMode(led, OUTPUT);
                digitalWrite(led, ledStatus);
                taskState = TaskStates::SLOW;
            }
            else if (buttonEvt.whichButton == BUTTONS::TWO_BTN)
            {
                if_off = true;
                ledStatus = LOW;
                pinMode(led, OUTPUT);
                digitalWrite(led, ledStatus);
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
                ledStatus = LOW;
                pinMode(led, OUTPUT);
                digitalWrite(led, ledStatus);
                taskState = TaskStates::WAIT_ON;
            }
            else if (buttonEvt.whichButton == BUTTONS::TWO_BTN)
            {
                ledStatus = LOW;
                pinMode(led, OUTPUT);
                digitalWrite(led, ledStatus);
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
            pinMode(led, OUTPUT);
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
                ledStatus = LOW;
                pinMode(led, OUTPUT);
                digitalWrite(led, ledStatus);
                taskState = TaskStates::MEDIUM;
            }
            else if (buttonEvt.whichButton == BUTTONS::TWO_BTN)
            {
                if_off = false;
                ledStatus = LOW;
                pinMode(led, OUTPUT);
                digitalWrite(led, ledStatus);
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
                    if (if_off == true)
                    {
                        ledStatus = HIGH;
                        pinMode(led, OUTPUT);
                        digitalWrite(led, ledStatus);
                        taskState = TaskStates::PERM_OFF; 
                    }
                    else
                    {
                        ledStatus = LOW;
                        pinMode(led, OUTPUT);
                        digitalWrite(led, ledStatus);
                        taskState = TaskStates::PERM_ON; 
                    }
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