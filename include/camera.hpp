#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "triangle.h"

class Camera {
    public:
    Camera();
    // glm::vec3 eye;
    // glm::vec3 view_direction;
    // glm::vec3 up_direction;

    glm::mat4 get_view_matrix() const;

    void MoveLeft(Distance distance);
    void MoveRight(Distance distance);
    void MoveUp(Distance distance);
    void MoveDown(Distance distance);
    void ZoomIn(Distance distance);
    void ZoomOut(Distance distance);

    private:
        glm::vec3 eye;
        glm::vec3 view_direction;
        glm::vec3 up_direction;
        void make_view_direction_negative_z(void);
};

#endif /* CAMERA_HPP */