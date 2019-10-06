#ifndef TEXTURE_H
#define TEXTURE_H

#include "ray.h"
#include "perlin.h"

class texture
{
public:
    virtual vec3 value(float u, float v, const vec3& p) const = 0;
};

class constant_texture : public texture
{
public:
    constant_texture() = default;
    constant_texture(vec3 c) : color(c) {}
    virtual vec3 value(float u, float v, const vec3& p) const
    {
        return color;
    }
    vec3 color;
};

class checker_texture : public texture
{
public:
    checker_texture() = default;
    checker_texture(texture* t0, texture* t1) : even(t0), odd(t1) {}
    virtual vec3 value(float u, float v, const vec3& p) const
    {
        float sines = sin(10 * p.x()) * sin(10 * p.y()) * sin(10 * p.z());
        if(sines < 0) return odd->value(u, v, p);
        else return even->value(u, v, p);
    }
    texture* odd;
    texture* even;
};

class noise_texture : public texture
{
public:
    noise_texture() = default;
    noise_texture(float sc) : scale(sc) {}
    virtual vec3 value(float u, float v, const vec3& p) const
    {
        //return vec3(noise.noise(scale * p));
        return vec3(0.5 * (1 + sin(scale * p.x() + 10 * noise.turb(p)))); //marble
        //return vec3(noise.turb(scale * p));
    }
    perlin noise;
    float scale = 1;
};

class image_texture : public texture
{
public:
    image_texture() = default;
    image_texture(unsigned char* pixels, int A, int B) : data(pixels), nx(A), ny(B) {}
    virtual vec3 value(float u, float v, const vec3& p) const
    {
        int i = u * nx;
        int j = (1 - v) * ny - 0.001;
        i = i < 0 ? 0 : (i > nx - 1 ? nx - 1 : i);
        j = j < 0 ? 0 : (j > ny - 1 ? ny - 1 : j);

        float r = int(data[3 * i + 3 * nx * j]) / 255.0;
        float g = int(data[3 * i + 3 * nx * j + 1]) / 255.0;
        float b = int(data[3 * i + 3 * nx * j + 2]) / 255.0;
        return vec3(r, g, b);
    }
    unsigned char* data;
    int nx, ny;
};
#endif