#include "camera.hpp"

#include <stdio.h>

Camera::Camera() {
    eye = glm::vec3(0.0f, 0.0f, 0.0f);
    make_view_direction_negative_z();
    up_direction = glm::vec3(0.0, 1.0f, 0.0f);
}

void Camera::make_view_direction_negative_z(void) {
    view_direction = glm::vec3(eye.x, eye.y, eye.z - 1.0f);
}

glm::mat4 Camera::get_view_matrix() const {
    return glm::lookAt(eye, view_direction, up_direction);
}

void Camera::MoveLeft(Distance distance) {
    eye.x -= distance;
    make_view_direction_negative_z();
    printf("move left;  eye = x %3.3f y %3.3f z %3.3f\n", eye.x, eye.y, eye.z);
}
void Camera::MoveRight(Distance distance) {
    eye.x += distance;
    make_view_direction_negative_z();
    printf("move right;  eye = x %3.3f y %3.3f z %3.3f\n", eye.x, eye.y, eye.z);
}
void Camera::MoveUp(Distance distance) {
    eye.y += distance;
    make_view_direction_negative_z();
    printf("move up;  eye = x %3.3f y %3.3f z %3.3f\n", eye.x, eye.y, eye.z);
}
void Camera::MoveDown(Distance distance) {
    eye.y -= distance;
    make_view_direction_negative_z();
    printf("move down;  eye = x %3.3f y %3.3f z %3.3f\n", eye.x, eye.y, eye.z);
}

void Camera::ZoomOut(Distance distance) {
    eye.z += distance;
    make_view_direction_negative_z();
    printf("zoom out;  eye = x %3.3f y %3.3f z %3.3f\n", eye.x, eye.y, eye.z);
}

void Camera::ZoomIn(Distance distance) {
    eye.z -= distance;
    make_view_direction_negative_z();
    printf("zoom in;  eye = x %3.3f y %3.3f z %3.3f\n", eye.x, eye.y, eye.z);
}