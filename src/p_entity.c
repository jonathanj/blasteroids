#include <stdlib.h>
#include <SDL2/SDL.h>

#include "p_entity.h"
#include "p_util.h"
#include "linked_list.h"
#include "m_message.h"

linked_list_node_t *entities = NULL;

const char *ENTITY_TYPE_NAMES[] = { "Player", "Asteroid" };

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

  if (entity->think) {
    entity->think(entity, state);
  }

  M_Physics_SimulateStep(&entity->contact_body, (const world_t *)&state->world, state->delta_time);
}

// bool check_aabb_collision(p_entity_t *a, p_entity_t *b) {
//   float ax = a->position.x, ay = a->position.y;
//   float bx = b->position.x, by = b->position.y;

//   float width = 10;
//   float height = 10;

//   return (
//     ax < bx + width &&
//     ax + width > bx &&
//     ay < by + height &&
//     ay + height > by);
// }

// bool P_EntityManager_CheckCircleCollision(p_contact_body_t *a, p_contact_body_t *b, p_contact_t *contact_out) {
//   if (a->radius > 0 && b->radius > 0) {
//     vec2_t diff = vec2_sub(&a->position, &b->position);
//     float dst = fabs(vec2_length(&diff));
//     if (dst < (a->radius + b->radius)) {
//       contact_out->a = a;
//       contact_out->b = b;
//       vec2_t normal = vec2_sub(&a->position, &b->position);
//       vec2_normalize(&normal);
//       contact_out->normal = normal;
//       return true;
//     }
//   }
//   return false;
// }


void P_EntityManager_CheckCollisions() {
  m_contact_t contact;
  for (linked_list_node_t *a = entities->next; a != NULL; a = a->next) {
    for (linked_list_node_t *b = a->next; b != NULL; b = b->next) {
      p_entity_t *entity_a = a->data;
      p_entity_t *entity_b = b->data;
      if (M_Physics_CheckCircleCollision(&entity_a->contact_body, &entity_b->contact_body, &contact)) {
        // FIXME: This doesn't really work for the player though, we don't want them to bounce off asteroids
        bool resolve_a = true;
        if (entity_a->collided) {
          resolve_a = entity_a->collided(entity_a, entity_b);
        }
        bool resolve_b = true;
        if (entity_b->collided) {
          resolve_b = entity_b->collided(entity_b, entity_a);
        }

        if (resolve_a && resolve_b) {
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

void P_EntityManager_Destroy(p_entity_t *entity) {
  if (entity == NULL) return;

  if (entity->destroy != NULL) {
    entity->destroy(entity->data);
  }
  // FIXME: This needs to find the linked list node for the entity.
  //linked_list_remove(entities, entity);

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
