#include "boid.h"

static const float BIRD_RANGE = 20.f;
static const float BIRD_SPEED = 10.f;
static const float ALIGNMENT_WEIGHT = 0.55f;
static const float COHESION_WEIGHT = 1.3f;
static const float SEPERATION_WEIGHT = 1.3f;
// static const float OBSTACLE_AVOIDANCE_WEIGHT = 0.075f;
// static const float GOAL_SEEKING_WEIGHT = 0.f;
static const float OBSTACLE_AVOIDANCE_WEIGHT = 0.f;
static const float GOAL_SEEKING_WEIGHT = 0.f;

static const float COUNTER_VELOCITY = 0.05f;
BoidBehavior::BoidBehavior(Entity * entity) {
    this->entity  = entity;
    perching = false;

}

BoidBehavior::~BoidBehavior() {}

glm::vec3 BoidBehavior::bound_position() {
    float ground = 0.0f;
    float Xmin = -50.f, Xmax = 50.f, Ymin = 1.5f, Ymax = 50.f, Zmin = -50.f, Zmax = 50.f;

    glm::vec3 pos = entity->get_position();
    glm::vec3 v(0.0f);

    if(pos.x < Xmin) {
        v.x = COUNTER_VELOCITY;
    } else if(pos.x > Xmax) {
        v.x = -COUNTER_VELOCITY;
    }
    
    // Check to see if we should perch
    if(pos.y < ground) {
        entity->_origin.y = ground;
        perching = true;
        perch_timer =  static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(10.f)));
    } 
    
    if (pos.y < Ymin) {
        v.y = COUNTER_VELOCITY;
    } else if(pos.y > Ymax) {
        v.y = -COUNTER_VELOCITY;
    }

    
    if(pos.z < Zmin) {
        v.z = COUNTER_VELOCITY;
    } else if(pos.z > Zmax) {
        v.z = -COUNTER_VELOCITY;
    }

    return v;
}

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

        float distance = glm::distance(neighbor_position, my_position);
        
        if (distance > BIRD_RANGE) continue;

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
            obstacle_avoidance += (neighbor_position - my_position);
            obstacle_count++;

            // std::cout << "Human nearby! " << distance << std::endl;

            if (distance < (BIRD_RANGE / 2.0f)) {
                perching = false;
                perch_timer = 0.0f;
            }
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
        + SEPERATION_WEIGHT * seperation
        + bound_position();

    if (perching) {
        if (perch_timer > 0.f) {
            perch_timer -= dt;

            // Make them look around but not up
            target_velocity.y = 0.0f;
        } else {
            perching = false;
            // Launch!
            target_velocity.y = 1.0f;
        }

        entity->set_direction(glm::normalize(target_velocity));
    } else {
        // entity->_direction = glm::normalize(target_velocity);
        entity->set_direction(glm::normalize(target_velocity));
        entity->_origin += dt * BIRD_SPEED * entity->_direction;
    }
}