#ifndef EVENT_SYSTEM_H
#define EVENT_SYSTEM_H
#include "event.h"
#ifdef TEST_ENABLE
#include "datastructures/queue_event.h"
#endif

void event_system_init();
void event_system_deinit();
void event_system_post(event e);
int  event_system_get(event* e);

#ifdef TEST_ENABLE
const queue_event* event_system_get_event_queue();
#endif


#endif
