#define GLM_FORCE_RADIANS
#define GLM_ENABLE_EXPERIMENTAL
#include "GeomTriangle.h"

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

#include "Intersection.h"
#include "Ray.h"

GeomTriangle::GeomTriangle(std::vector<glm::vec3> &vertices, std::vector<glm::vec3> &normals) {
    this->vertices[0] = vertices[0];
    this->vertices[1] = vertices[1];
    this->vertices[2] = vertices[2];

    this->normals[0] = normals[0];
    this->normals[1] = normals[1];
    this->normals[2] = normals[2];
}

std::vector<Intersection> GeomTriangle::intersect(Ray &ray) {
    using namespace glm;
    /**
     * NOTE: Ray is already transformed to the Model coordinate space.
     */
    // vector to store the intersections
    std::vector<Intersection> intersections;
    
    /**
     * Implementation of the Möller-Trumbore algorithm for ray-triangle intersection
     */
    

    vec3 v0 = vertices[0];
    vec3 v1 = vertices[1];
    vec3 v2 = vertices[2];
    //eedges
    vec3 e1 = v1 - v0;
    vec3 e2 = v2 - v0;
  
    vec3 p = cross(ray.dir, e2);
    
    float det = dot(e1, p);
  
    if (det > -1e-8 && det < 1e-8)
        return intersections;
    float inv_det = 1.0f / det;
   
    vec3 t_vec = ray.p0 - v0;
    float u = dot(t_vec, p) * inv_det;
    
    if (u < 0.0f || u > 1.0f)
        return intersections;
    
    vec3 q = cross(t_vec, e1);
    
    float v = dot(ray.dir, q) * inv_det;
    
    if (v < 0.0f || u + v > 1.0f)
        return intersections
    
    float t = dot(e2, q) * inv_det;
    
    if (t > 0) {
        
        vec3 point = ray.p0 + t * ray.dir;
        float w = 1.0f - u - v;
        vec3 normal = normalize(w * normals[0] + u * normals[1] + v * normals[2]);
        
        intersections.push_back({t, point, normal, this, nullptr});
    }
    
    return intersections;
}