#include "Camera.h"

glm::mat3 rotation(const float degrees, const glm::vec3 axis) {
    const float angle = degrees * M_PI / 180.0f;
    glm::vec3 a = glm::normalize(axis);

    float cos_theta = cos(angle);
    float sin_theta = sin(angle);
    glm::mat3 K(0.0f);
    K[1][0] = a.z;    
    K[2][0] = -a.y;
    K[0][1] = -a.z;   
    K[2][1] = a.x;
    K[0][2] = a.y;   
    K[1][2] = -a.x;

    glm::mat3 I(1.0f);
    return I + sin_theta * K + (1.0f - cos_theta) * K * K;
}

void Camera::rotateRight(const float degrees) {
    glm::vec3 view_dir = eye - target;
    up = glm::normalize(up);
    glm::mat3 R = rotation(-degrees, up);
    
    view_dir = R * view_dir;
    eye = target + view_dir;
}

void Camera::rotateUp(const float degrees) {
    
    glm::vec3 view_dir = eye - target;
    up = glm::normalize(up);
    glm::vec3 right = glm::normalize(glm::cross(view_dir, up));
    glm::mat3 R = rotation(-degrees, right);  
    
    view_dir = R * view_dir;
    up = glm::normalize(R * up);
    eye = target + view_dir;
}

void Camera::computeMatrices() {
    glm::vec3 w = glm::normalize(eye - target);
    up = glm::normalize(up);
    glm::vec3 u = glm::normalize(glm::cross(up, w)); 
    glm::vec3 v = glm::normalize(glm::cross(w, u));  

    view = glm::mat4(
        u.x, v.x, w.x, 0.0f,
        u.y, v.y, w.y, 0.0f,
        u.z, v.z, w.z, 0.0f,
        -glm::dot(u, eye), -glm::dot(v, eye), -glm::dot(w, eye), 1.0f
    );
    
    float fovy_rad = fovy * M_PI / 180.0f;
    float f = 1.0f / tan(fovy_rad / 2.0f);
    
    
    proj = glm::mat4(0.0f);
    proj[0][0] = f / aspect;
    proj[1][1] = f;
    proj[2][2] = (farDist + nearDist) / (nearDist - farDist);
    proj[3][2] = (2.0f * farDist * nearDist) / (nearDist - farDist);
    proj[2][3] = -1.0f;
}

void Camera::reset() {
    eye = eye_default;
    target = target_default;
    up = glm::normalize(up_default);
    fovy = fovy_default;
    aspect = aspect_default;
    nearDist = near_default;
    farDist = far_default;
}