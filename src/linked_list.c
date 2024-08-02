#include <SDL2/SDL.h>

#include "linked_list.h"
#include "m_message.h"

linked_list_node_t *linked_list_new(void *data) {
  linked_list_node_t *node = malloc(sizeof(linked_list_node_t));
  if (node == NULL) {
    M_Log("[Util/LinkedList] Unable to create node\n");
    return NULL;
  }

  node->next = NULL;
  node->data = data;
  return node;
}

void linked_list_append(linked_list_node_t *head, void *data) {
  // if (head == NULL) {
  //   *head = linked_list_new(data);
  //   return *head;
  // }
  SDL_assert(head != NULL);

  linked_list_node_t *current = head;
  while (current->next != NULL) {
    current = current->next;
  }
  current->next = linked_list_new(data);
}

void linked_list_remove(linked_list_node_t *head, linked_list_node_t *node) {
  SDL_assert(head != NULL);
  SDL_assert(node != NULL);
  SDL_assert(node != head);

  linked_list_node_t *current = head;
  while (current != NULL) {
    if (current->next == node) {
      current->next = node->next;
      free(node);
      break;
    }
  }
}

void linked_list_iterate(linked_list_node_t *head, void(*func)(void *data, void *extra), void *extra) {
  SDL_assert(func != NULL);

  linked_list_node_t *current = head;
  while (current != NULL) {
    if (current->data != NULL) {
      func(current->data, extra);
    }
    current = current->next;
  }
}

void linked_list_free(linked_list_node_t *head) {
  linked_list_node_t *next = head;
  while (next != NULL) {
    linked_list_node_t *current = next;
    next = current->next;
    if (current != NULL) {
      free(current);
    }
  }
}
