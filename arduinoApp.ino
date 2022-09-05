#include "task1.h"
#include "task2.h"
#include "task3.h"
#include "events.h"

buttonEvt_t buttonEvt = {false, BUTTONS::NONE};
void setup()
{
    task1();
    task2();
    task3();
}

void loop()
{
    task1();
    task2();
    task3();
}
