#include "boid.h"

static const float BIRD_RANGE = 10.f;
static const float BIRD_SPEED = 8.f;
static const float ALIGNMENT_WEIGHT = 0.65f;
static const float COHESION_WEIGHT = 1.3f;
static const float SEPERATION_WEIGHT = 1.1f;
static const float OBSTACLE_AVOIDANCE_WEIGHT = 0.075f;
static const float GOAL_SEEKING_WEIGHT = 0.1f;


BoidBehavior::BoidBehavior(Entity * entity) {
    this->entity  = entity;
}

BoidBehavior::~BoidBehavior() {}

void BoidBehavior::update(float dt) {
    std::vector<Entity *> nearby = Globals::scene->get_nearby_entities(entity, BIRD_RANGE);

    size_t nearby_count = 0;
    size_t obstacle_count = 0;
    size_t goal_count = 0;

    glm::vec3 alignment(0.f);
    glm::vec3 cohesion(0.f);
    glm::vec3 seperation(0.f);

    glm::vec3 my_position = entity->get_position();

    glm::vec3 obstacle_avoidance(0.f);

    // They all generally want to stay near the map, but weighted less than the player
    glm::vec3 goal_seeking(glm::vec3(0.f, 20.f, 0.f) - my_position);
    goal_seeking = 0.1f * glm::normalize(goal_seeking);

    for(auto t=nearby.begin(); t!=nearby.end(); ++t) {
        if ((*t) == entity) continue;

        glm::vec3 neighbor_position = (*t)->get_position();

        if (glm::distance(neighbor_position, my_position) > BIRD_RANGE) continue;

        if ((*t)->get_type() == BirdEntity) {
            alignment += (*t)->get_direction();
            cohesion += neighbor_position;
            seperation += (neighbor_position - my_position);
            nearby_count++;
        }

        if ((*t)->get_type() == ObstacleEntity) {
            obstacle_avoidance += (neighbor_position - my_position);
            obstacle_count++;
        }

        if ((*t)->get_type() == PlayerEntity) {
            goal_seeking += ((neighbor_position + glm::vec3(0.f, 3.f, 0.f)) - my_position);
            goal_count++;
        }
    }

    if (nearby_count > 0) {
        alignment = glm::normalize(alignment / (float) nearby_count);
        cohesion = glm::normalize((cohesion / (float) nearby_count) - my_position);
        seperation = glm::normalize(-1.f * seperation);
    }

    if (obstacle_count > 0) {
        obstacle_avoidance = glm::normalize(-1.f * obstacle_avoidance);
    }

    if (goal_count > 0) {
        goal_seeking = glm::normalize(goal_seeking);
    }

    glm::vec3 target_velocity = entity->_direction 
        + GOAL_SEEKING_WEIGHT * goal_seeking
        + ALIGNMENT_WEIGHT * alignment 
        + COHESION_WEIGHT * cohesion 
        + OBSTACLE_AVOIDANCE_WEIGHT * obstacle_avoidance 
        + SEPERATION_WEIGHT * seperation;
    
    if (my_position.y < 1.0f) target_velocity.y = 0.1f;
    if (my_position.y > 50.f) target_velocity.y = -0.1f;

    entity->set_direction(glm::normalize(target_velocity));

    entity->_origin += dt * BIRD_SPEED * entity->_direction;
}