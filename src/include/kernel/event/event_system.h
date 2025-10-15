#ifndef EVENT_SYSTEM_H
#define EVENT_SYSTEM_H
#include "event.h"

void event_system_init();
void event_system_deinit();
void event_system_post(event e);
int  event_system_get(event* e);

#endif
