#include "gengen2.h"

int main() {
  ctemplate queue = template_create("queue");
  template_addfile(&queue, "queue.htpl", "queue_$T.h");
  template_addfile(&queue, "queue.ctpl", "queue_$T.c");
  template_addreplacement(&queue, "$T", NULL);
  template_addreplacement(&queue, "^T", NULL);
  template_addreplacement(&queue, "SUPPORT_1", NULL);
  template_addreplacement(&queue, "REALLOC", NULL);
  template_addreplacement(&queue, "CALLOC", NULL);
  template_addreplacement(&queue, "FREE", NULL);

  ctemplate linkedlist = template_create("linkedlist");
  template_addfile(&linkedlist, "linkedlist.htpl", "linkedlist_$T.h");
  template_addfile(&linkedlist, "linkedlist.ctpl", "linkedlist_$T.c");

  replacement event_queue_repl = replacement_create();
  replacement_add(&event_queue_repl, "$T", "event");
  replacement_add(&event_queue_repl, "^T", "event");
  replacement_add(&event_queue_repl, "SUPPORT_1", "event/event.h");
  replacement_add(&event_queue_repl, "REALLOC", "realloc");
  replacement_add(&event_queue_repl, "CALLOC", "calloc");
  replacement_add(&event_queue_repl, "FREE", "free");

  replacement memory_linked_list_repl = replacement_create();
  replacement_add(&memory_linked_list_repl, "$T", "memory_node");
  replacement_add(&memory_linked_list_repl, "^T", "memory_node");
  replacement_add(&memory_linked_list_repl, "SUPPORT_1", "memory/memory_node.h");
  replacement_add(&memory_linked_list_repl, "REALLOC", "realloc");
  replacement_add(&memory_linked_list_repl, "CALLOC", "calloc");
  replacement_add(&memory_linked_list_repl, "FREE", "free");

  generator_run(gen_settings(.search_paths=paths("src/tpl"), .source_outdir="src/kernel/datastructures", .header_outdir="src/include/kernel/datastructures", .verbose = false), queue, event_queue_repl);
  generator_run(gen_settings(.search_paths=paths("src/tpl"), .source_outdir="src/kernel/datastructures", .header_outdir="src/include/kernel/datastructures", .verbose = false), linkedlist, memory_linked_list_repl);
}
