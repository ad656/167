#define GLM_ENABLE_EXPERIMENTAL
#include "GlossyMaterial.h"

#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

using namespace glm;

Ray GlossyMaterial::sample_ray_and_update_radiance(Ray &ray, Intersection &intersection) {
    /**
     * Calculate the next ray after intersection with the model.
     * This will be used for recursive ray tracing.
     */

    // Decide if diffuse or specular reflection
    float random = linearRand(0.0f, 1.0f);
    vec3 normal = intersection.normal;
    vec3 point = intersection.point;

    // Diffuse reflection
    if (random > shininess) {
        // Step 1: Sample ray direction
        /**
         * Implement cosine-weighted hemisphere sampling
         */
        // cosine sample next ray
        float s = linearRand(0.0f, 1.0f);
        float t = linearRand(0.0f, 1.0f);

        // Update u, v based on Equation (8) in handout
        float u = 2.0f * M_PI * s;
        float v = sqrt(1.0f - t);
        
        // Cosine-weighted sampled direction
        vec3 hemisphere_sample = vec3(
            cos(u) * v,
            sin(u) * v,
            sqrt(t)
        );

        // The direction we sampled above is in local co-ordinate frame
        // we need to align it with the surface normal
        vec3 new_dir = align_with_normal(hemisphere_sample, normal);

        // Step 2: Calculate radiance
        /**
         * C_diffuse = `this->diffuse`
         * For diffuse BRDF, radiance = C_diffuse / π
         */
        vec3 W_diffuse = diffuse / (float)M_PI;  // Diffuse BRDF (Lambert's law)

        // update radiance
        ray.W_wip = ray.W_wip * W_diffuse;

        // update ray direction and position
        ray.p0 = point + 0.001f * normal;  // offset point slightly to avoid self intersection
        ray.dir = new_dir;
        ray.is_diffuse_bounce = true;
        ray.n_bounces++;

        return ray;
    }

    // Specular Reflection

    // Step 1: Calculate reflection direction
    /**
     * Calculate the perfect mirror reflection direction
     * R = I - 2(N·I)N where I is incident direction, N is normal
     */
    vec3 incident = -normalize(ray.dir);  // Incident direction (negated ray direction)
    vec3 reflection_dir = normalize(2.0f * dot(normal, incident) * normal - incident);

    // Step 2: Calculate radiance
    /**
     * For specular BRDF, radiance = C_specular
     */
    vec3 W_specular = specular;

    // update radiance
    ray.W_wip = ray.W_wip * W_specular;
    ray.p0 = point + 0.001f * normal;  // offset point slightly to avoid self intersection
    ray.dir = reflection_dir;
    ray.is_diffuse_bounce = false;
    ray.n_bounces++;

    return ray;
}

glm::vec3 GlossyMaterial::get_direct_lighting(Intersection &intersection, Scene const &scene) {
    using namespace glm;

    /**
     * Note:
     * - Light sources from scene can be accessed by `scene.light_sources`
     * - Models from scene can be accessed by `scene.models`
     */

    // Iterate over all light sources
    vec3 cummulative_direct_light = vec3(0.0f);
    for (unsigned int idx = 0; idx < scene.light_sources.size(); idx++) {
        // intersection could be with one of the light source to
        // so skip self intersection
        if (scene.light_sources[idx] == intersection.model)
            continue;

        // get light source position
        vec3 light_pos = scene.light_sources[idx]->get_surface_point();

        // check if point is in shadow
        
        /**
         * 
         * TODO: Task 4.1
         * Shoot a shadow ray towards light source
         * Note:
         * - Offset ray starting position by small amount to avoid self shadowing
         * - Use `light_pos` and `intersection.point` to get direction for shadow ray
         * - Surface normal at point of intersection is stored in `intersection.normal`
         */
        Ray shadow_ray;
        shadow_ray.p0 = intersection.point + 0.001f * intersection.normal;
        // Direction from intersection point to light source
        shadow_ray.dir = normalize(light_pos - shadow_ray.p0);  // TODO: Update ray direction here

        // check if shadow ray intersects any model
        for (unsigned int idx = 0; idx < scene.models.size(); idx++)
            scene.models[idx]->intersect(shadow_ray);

        // get closest intersection
        Intersection closest_intersection;
        closest_intersection.t = std::numeric_limits<float>::max();
        for (unsigned int idx = 0; idx < shadow_ray.intersections.size(); idx++) {
            if (shadow_ray.intersections[idx].t < closest_intersection.t)
                closest_intersection = shadow_ray.intersections[idx];
        }

        // check if light source is visible
        if (closest_intersection.model == scene.light_sources[idx]) {
            // light source emission value
            vec3 light_emission = scene.light_sources[idx]->material->emission;

            /**
             * TODO: Task 4.1
             * Calculate direct light contribution using lamberts cosine law.
             * Refer Equation (3) in final project handout for more details
             *
             * NOTE:
             * - Surface normal at point of intersection is stored in `intersection.normal`
             * - This `if` condition block takes care of `visibility_of_light` part in the equation
             *   So here you just need to calculate contribution of light like we did in HW3 for diffuse part
             */
            vec3 light_dir = normalize(light_pos - intersection.point);
            float cos_theta = max(0.0f, dot(intersection.normal, light_dir));
            vec3 direct_light = light_emission * diffuse * (1.0f - shininess) * cos_theta; // TODO: Update direct light constribution of light source

            // attenuation factor for light source based on distance
            float attenuation_factor = scene.light_sources[idx]->material->get_light_attenuation_factor(closest_intersection.t);

            cummulative_direct_light += direct_light / attenuation_factor;
        }
    }

    return cummulative_direct_light;
}

vec3 GlossyMaterial::color_of_last_bounce(Ray &ray, Intersection &intersection, Scene const &scene) {
    using namespace glm;
    /**
     * Color after last bounce will be `W_wip * last_bounce_color`
     * We shade last bounce for this Glossy material using direct diffuse lighting
     */

    vec3 direct_diff_light = this->get_direct_lighting(intersection, scene);

    return ray.W_wip * diffuse * (1 - shininess) * direct_diff_light;
}