#include "event_system.h"
#include "log.h"
#include "queue_event.h"

static queue_event event_queue = {0};

void event_system_init() {
  event_queue = queue_event_create();
}
void event_system_deinit() {
  queue_event_free(&event_queue);
}
void event_system_post(event e) {
  if (event_queue.size >= 10) {
    return; // ignore events after we get too many
  }
  queue_event_enqueue(&event_queue, e);
  log_info("EventSystem", "Post event: #%d", event_queue.size);
}
bool event_system_get(event* e) {
  result_queue_event_dequeue r = queue_event_dequeue(&event_queue);
  if (r.isok && e) {
    *e = r.ok;
    return true;
  }
  return false;
}
