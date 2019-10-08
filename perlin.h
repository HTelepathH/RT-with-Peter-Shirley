#ifndef PERLIN_H
#define PERLIN_H

#include "vec3.h"
#include "rand.h"

inline float hermite_cubic(float a)
{
    return a * a * (3 - 2 * a);
}

inline float perlin_interp(vec3 c[2][2][2], float u, float v, float w)
{
    float uu = hermite_cubic(u);
    float vv = hermite_cubic(v);
    float ww = hermite_cubic(w);
    float accum = 0;
    for(int i = 0; i < 2; ++i)
        for(int j = 0; j < 2; ++j)
            for(int k = 0; k < 2; ++k)
            {
                vec3 weight_v(u - i, v - j, w - k);
                accum += (i * uu + (1 - i) * (1 - uu)) *
                         (j * vv + (1 - j) * (1 - vv)) *
                         (k * ww + (1 - k) * (1 - ww)) * dot(c[i][j][k], weight_v);
            }
    return fabs(accum);
}

class perlin
{
public:
    float noise(const vec3& p) const
    {
        int i = floor(p.x());
        int j = floor(p.y());
        int k = floor(p.z());
        float u = p.x() - floor(p.x());
        float v = p.y() - floor(p.y());
        float w = p.z() - floor(p.z());

        vec3 c[2][2][2];
        for(int di = 0; di < 2; ++di)
            for(int dj = 0; dj < 2; ++dj)
                for(int dk = 0; dk < 2; ++dk)
                    c[di][dj][dk] = ranvec[perm_x[(i + di) & 255] ^ perm_y[(j + dj) & 255] ^ perm_z[(k + dk) & 255]];

        return perlin_interp(c, u, v, w);
    }
    float turb(const vec3& p, int depth = 7) const 
    {
        float accum = 0;
        vec3 temp_p = p;
        float weight = 1.0;
        for (int i = 0; i < depth; ++i) {
            accum += weight * noise(temp_p);
            weight *= 0.5;
            temp_p *= 2;
        }
        return fabs(accum);
    }
    static vec3* ranvec;
    static int* perm_x;
    static int* perm_y;
    static int* perm_z;
};

static vec3* perlin_generate()
{
    vec3* p = new vec3[256];
    for (int i = 0; i < 256; ++i)
        p[i] = unit_vector(vec3(-1 + 2 * random(), -1 + 2 * random(), -1 + 2 * random()));
    return p;
}

void permute(int* p, int n) // swap randomly
{
    for(int i = n - 1; i > 0; --i)
    {
        int target = int(random() * (i + 1));
        std::swap(p[target], p[i]);
    }
}

static int* perlin_generate_perm()
{
    int* p = new int[256];
    for(int i = 0; i < 256; ++i)
        p[i] = i;
    permute(p, 256);
    return p;
}

vec3* perlin::ranvec = perlin_generate();
int* perlin::perm_x = perlin_generate_perm();
int* perlin::perm_y = perlin_generate_perm();
int* perlin::perm_z = perlin_generate_perm();

#endif