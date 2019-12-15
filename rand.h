// generate random number
#ifndef RAND_H 
#define RAND_H

inline float random() {
    return (float)(rand() / float(RAND_MAX));
}

inline vec3 random_in_unit_sphere()
{
    vec3 p;
    do {
        p = 2.0 * vec3(random(), random(), random()) - vec3(1.0);
    } while (p.squared_length() >= 1.0);
    return p;
}

inline vec3 random_in_unit_disk()
{
    vec3 p;
    do {
        p = 2.0 * vec3(random(), random(), 0) - vec3(1, 1, 0);
    } while (dot(p, p) >= 1.0);
    return p;
}

#endif