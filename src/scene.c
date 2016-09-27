#include "scene.h"

double sphere_intersection(ray ray, sceneObj obj) {
    double denominator = vector3d_dot(obj.plane.normal, ray.dir);
    if(denominator == 0) {
        return -1;
    }
    double t = - vector3d_dot(obj.plane.normal,
        vector3d_sub(ray.origin, obj.plane.pos)) / denominator;

    if(t > 0) {
        return t;
    }

    return -1;
}

double plane_intersection(ray ray, sceneObj obj) {
    double t = vector3d_dot(ray.dir, vector3d_sub(obj.sphere.pos, ray.origin));
    vector3d point = vector3d_add(ray.origin, vector3d_scale(ray.dir, t));
    double magnitude = vector3d_magnitude(vector3d_sub(point, obj.sphere.pos));
    if(magnitude > obj.sphere.radius) {
        return -1;
    }
    else if(magnitude < obj.sphere.radius) {
        double a = sqrt(pow(obj.sphere.radius, 2) - pow(magnitude, 2));

        return t - a;
    }
    else {
        return t;
    }
}

double cylinder_intersection(ray ray, sceneObj obj) {
    // Step 1. Find the equation for the object you are innterested in
    // x^2 + y^2 = r^2
    //
    // Step 2. Paramaterize the equation with a center point if needed
    // (x - Cx)^2 + (z - Cz)^2 = r^2
    //
    // Step 3. Substitute the eq for a ray into our object equation.
    // (Rox + t * Rdx - Cx)^2 + (Roz + t * Rdz - Cz)^2 - r^2 = 0
    //
    // Step 4. Solve for t.
    //
    // Step 4a. Rewrite the equation (flatten).
    // -r^2 +
    // t^2 * Rdx^2 +
    // t^2 * Rdz^2 +
    // 2*t * Rox * Rdx -
    // 2*t * Rdz * Cx +
    // 2*t * Roz * Rdz -
    // 2*t * Rdz * Cz +
    // Rox^2 -
    // 2*Rox*Cx +
    // Cx^2 +
    // Roz^2 -
    // 2*Roz*Cz +
    // Cz^2 = 0
    //
    // Step 4b. Rewrite the equation in terms of t.
    // t^2 * (Rdx^2 + Rdz^2) +
    // t * (2 * (Rox * Rdx - Rdz * Cx + Roz * Rdz - Rdz * Cz)) +
    // Rox^2 - 2*Rox*Cx + Cx^2 + Roz^2 - 2*Roz*Cz + Cz^2 = 0
    //
    // Use the quadratic equation to solve for t
    //

    double a = pow(ray.dir.x, 2) + pow(ray.dir.x, 2);
    double b = 2 * (
        ray.origin.x * ray.dir.x -
        ray.dir.z * obj.cylinder.pos.x +
        ray.origin.z * ray.dir.z -
        ray.dir.z * obj.cylinder.pos.z
    );
    double c = pow(ray.origin.z, 2) -
        2 * ray.origin.x * obj.cylinder.pos.x +
        pow(obj.cylinder.pos.x, 2) + pow(ray.origin.z, 2) -
        2 * ray.origin.z * obj.cylinder.pos.z +
        pow(obj.cylinder.pos.z, 2);

    double determinant = pow(b, 2) - 4 * a *c;
    if (determinant < 0) {
        return -1;
    }

    determinant = sqrt(determinant);

    double t0 = (-b - determinant) / (2 * a);
    if(t0 > 0) {
        return t0;
    }

    double t1 = (-b + determinant) / (2 * a);
    if (t1 > 0) {
        return t1;
    }

    return -1;
}
