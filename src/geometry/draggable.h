#ifndef DRAGGABLE_H
#define DRAGGABLE_H

#include <glm/vec3.hpp> 

// Base class
class Draggable {
   public:
      virtual glm::vec3 get_position() = 0;
      virtual void set_position(glm::vec3 position) = 0;
};

#endif