#ifndef LINKED_LIST_H
#define LINKED_LIST_H

typedef struct linked_list_node_t linked_list_node_t;

struct linked_list_node_t {
  struct linked_list_node_t *next;
  void *data;
};

linked_list_node_t *linked_list_new(void *data);
void linked_list_append(linked_list_node_t *head, void *data);
void linked_list_remove(linked_list_node_t *head, linked_list_node_t *node);
void linked_list_remove_by_data(linked_list_node_t *head, void *data);
void linked_list_iterate(linked_list_node_t *head, void(*func)(void *data, void *extra), void *extra);
void linked_list_free(linked_list_node_t *head);

#endif
