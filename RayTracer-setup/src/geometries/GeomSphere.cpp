#include "GeomSphere.h"

#include <iostream>
#include <utility>

#include "Intersection.h"
#include "Ray.h"

std::vector<Intersection> GeomSphere::intersect(Ray &ray) {
    /**
     * NOTE: Ray is already transformed to the Model coordinate space.
     */
    using namespace glm;
    // vector to store the intersections
    std::vector<Intersection> intersections;
    
    /**
     * TODO: Implement the Ray intersection with the current geometry
     */
    
    
    float a = dot(ray.dir, ray.dir);
    float b = 2.0f * dot(ray.p0, ray.dir);
    float c = dot(ray.p0, ray.p0) - 1.0f;  
    
 
    float discriminant = b * b - 4 * a * c;
    
    
    if (discriminant < 0) {
        return intersections;
    }
    float sqrtDiscriminant = sqrt(discriminant);
    float t1 = (-b - sqrtDiscriminant) / (2 * a);
    float t2 = (-b + sqrtDiscriminant) / (2 * a);
    if (t1 > 0) {
        
        vec3 point = ray.p0 + t1 * ray.dir;
        
      
        vec3 normal = normalize(point);
        
        
        intersections.push_back({t1, point, normal, this, nullptr});
    }
    
    
    if (t2 > 0 && t1 != t2) {
        vec3 point = ray.p0 + t2 * ray.dir;
        vec3 normal = normalize(point);
        intersections.push_back({t2, point, normal, this, nullptr});
    }
    
    return intersections;
}