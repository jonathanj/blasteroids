#include <stdlib.h>
#include <SDL2/SDL.h>

#include "p_entity.h"
#include "p_util.h"
#include "linked_list.h"
#include "m_message.h"

linked_list_node_t *entities = NULL;

const char *ENTITY_TYPE_NAMES[] = { "Player", "Asteroid", "Bullet" };

const char *P_EntityManager_NameByType(p_entity_type_t type) {
  return ENTITY_TYPE_NAMES[type];
}

void P_EntityManager_Init() {
  entities = linked_list_new(NULL);
}

p_entity_t *P_EntityManager_Spawn(p_entity_type_t type, m_contact_body_t body, double dir_angle) {
  p_entity_t *entity = malloc(sizeof(p_entity_t));
  if (entity == NULL) {
    M_Log("[Entity/Manager] Unable to spawn entity\n");
    return NULL;
  }
  entity->type = type;
  entity->dir_angle = dir_angle;
  entity->contact_body = body;
  entity->dead = false;
  entity->destroy = NULL;
  entity->think = NULL;
  entity->collided = NULL;
  entity->render = NULL;

  linked_list_append(entities, entity);

  return entity;
}

void P_EntityManager_ThinkEntity(void *data, void *extra) {
  p_entity_t *entity = (p_entity_t *)data;
  SDL_assert(entity != NULL);
  game_state_t *state = (game_state_t *)extra;
  SDL_assert(state != NULL);

  if (!entity->dead && entity->think) {
    entity->think(entity, state);
  }

  if (!entity->dead) {
    M_Physics_SimulateStep(&entity->contact_body, (const world_t *)&state->world, state->delta_time);
  }
}

void P_EntityManager_CheckCollisions() {
  m_contact_t contact;
  for (linked_list_node_t *a = entities->next; a != NULL; a = a->next) {
    for (linked_list_node_t *b = a->next; b != NULL; b = b->next) {
      p_entity_t *entity_a = a->data;
      p_entity_t *entity_b = b->data;
      if (entity_a->dead || entity_b->dead) {
        continue;
      }

      if (M_Physics_CheckCircleCollision(&entity_a->contact_body, &entity_b->contact_body, &contact)) {
        bool resolve_a = true;
        if (entity_a->collided) {
          resolve_a = entity_a->collided(entity_a, entity_b, &contact);
        }
        bool resolve_b = true;
        if (entity_b->collided) {
          resolve_b = entity_b->collided(entity_b, entity_a, &contact);
        }

        if (resolve_a && resolve_b) {
          M_Log("[Entity/Manager] %s collided with %s\n", P_EntityManager_NameByType(entity_a->type), P_EntityManager_NameByType(entity_b->type));
          M_Physics_ResolveCollision(&contact);
        }
      }
    }
  }
}

void P_EntityManager_Think(const game_state_t *state) {
  linked_list_iterate(entities, P_EntityManager_ThinkEntity, (void *)state);

  P_EntityManager_CheckCollisions();
}

void P_EntityManager_RenderEntity(void *data, void *extra SDL_UNUSED) {
  p_entity_t *entity = (p_entity_t *)data;
  SDL_assert(entity != NULL);
  if (entity->render) {
    entity->render(entity);
  }
}

void P_EntityManager_Render() {
  linked_list_iterate(entities, P_EntityManager_RenderEntity, NULL);
}

// FIXME: this has a bug where two adjacent dead entities are incorrecty swept
void P_EntityManager_Sweep() {
  linked_list_node_t *last = entities;
  linked_list_node_t *current = entities->next;
  while (current != NULL) {
    if (current->data != NULL) {
      p_entity_t *entity = (p_entity_t *)current->data;
      if (entity->dead) {
        entity->destroy(entity->data);
        last->next = current->next;
        // TODO: Perhaps the linked_list code should free stuff it allocs?
        free(entity);
        // NOTE: `last` is intentionally not updated, to avoid leaving dead
        // entity pointers in the list.
        current = current->next;
        continue;
      }
    }
    last = current;
    current = current->next;
  }
}

void P_EntityManager_Destroy(p_entity_t *entity) {
  if (entity == NULL) return;

  if (entity->destroy != NULL) {
    entity->destroy(entity->data);
  }
  if (entities != NULL) {
    linked_list_remove_by_data(entities, entity);
  }

  // TODO: Removing this thing in the middle of a frame is probably not a good
  // idea, instead there should be some bookkeeping and then have G_FrameEnd
  // clean it up.
}

void P_EntityManager_Shutdown() {
  linked_list_node_t *current = entities;
  while ((current = current->next) != NULL) {
    p_entity_t *entity = current->data;
    if (entity != NULL && entity->destroy != NULL) {
      entity->destroy(entity->data);
    }
  }

  linked_list_free(entities);
}
