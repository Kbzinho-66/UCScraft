#ifndef UCSCRAFT_BLOCK_H
#define UCSCRAFT_BLOCK_H

#include <glm/vec3.hpp>

class Block {

public:
    glm::vec3 Position;
    int Texture;

    Block(glm::vec3 position, int texture) {
        Position = position;
        Texture = texture;
    }
};


#endif //UCSCRAFT_BLOCK_H
