#ifndef MATERIAL_H
#define MATERIAL_H

#include "hitable.h"
#include "rand.h"
#include "texture.h"

class material 
{
public:
    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const = 0;
    virtual vec3 emitted(float u, float v, const vec3& p) const 
    {
        return vec3(0);
    }
};

// reflect and refract ---------------------------------------------------------------------------------------------
inline vec3 reflect(const vec3& v, const vec3& n)
{
    return v - 2 * dot(v, n) * n;
}

inline bool refract(const vec3& v, const vec3& n, float ni_over_nt, vec3& refracted)
{
    vec3 uv = unit_vector(v);
    float dt = dot(uv, n); // in_cos
    float discriminant = 1.0 - ni_over_nt * ni_over_nt * (1 - dt * dt); // out_sin^2
    if(discriminant > 0)
    {
        refracted = ni_over_nt * (uv - n * dt) - n * sqrt(discriminant);
        return true;
    }
    else return false;
}

//lambertian-------------------------------------------------------------------------------------------------------------
class lambertian : public material
{
public:
    lambertian(texture* a) : albedo(a) {}
    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const
    {
        vec3 target = rec.p + rec.normal + random_in_unit_sphere();
        scattered = ray(rec.p, target - rec.p, r_in.time());
        attenuation = albedo->value(rec.u, rec.v, rec.p);
        return true;
    }
    texture* albedo;
};

//metal-------------------------------------------------------------------------------------------------------------
class metal : public material
{
public:
    metal(const vec3& a, float f) : albedo(a) 
    {
        fuzz = f < 1 ? f : 1;
    }
    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const
    {
        vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
        scattered = ray(rec.p, reflected + fuzz * random_in_unit_sphere(), r_in.time());
        attenuation = albedo;
        return (dot(scattered.direction(), rec.normal) > 0);
    }
    vec3 albedo;
    float fuzz;
};

//schlick-----------------------------------------------------------------------------------------------------------
float schlick(float cosine, float ref_idx) //cosine : in_cosine
{
    float r0 = (1 - ref_idx) / (1 + ref_idx);
    r0 = r0 * r0;
    return r0 + (1 - r0) * pow(1 - cosine, 5);
}

//dielectric----------------------------------------------------------------------------------------------------------
class dielectric : public material
{
public:
    dielectric(float ri) : ref_idx(ri) {}
    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const
    {
        vec3 outward_normal;
        vec3 reflected = reflect(r_in.direction(), rec.normal);
        float ni_over_nt;
        attenuation = vec3(1.0);
        vec3 refracted;
        float reflect_prob;
        float cosine;
        if(dot(r_in.direction(), rec.normal) > 0)
        {
            outward_normal = -rec.normal;
            ni_over_nt = ref_idx;
            cosine = ref_idx * dot(r_in.direction(), rec.normal) / r_in.direction().length();
        }
        else {
            outward_normal = rec.normal;
            ni_over_nt = 1 / ref_idx;
            cosine = -dot(r_in.direction(), rec.normal) / r_in.direction().length();
        }

        if(refract(r_in.direction(), outward_normal, ni_over_nt, refracted))
        {
            reflect_prob = schlick(cosine, ref_idx);
        }
        else {
            reflect_prob = 1.0;
        }

        if(random() < reflect_prob)
        {
            scattered = ray(rec.p, reflected, r_in.time());
        }
        else {
            scattered = ray(rec.p, refracted, r_in.time());
        }
        return true;
    }
    float ref_idx;
};

//diffuse_light----------------------------------------------------------------------------------------------------------
class diffuse_light : public material
{
public:
    diffuse_light() = default;
    diffuse_light(texture* a) : emit(a) {}
    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const
    {
        return false;
    }
    virtual vec3 emitted(float u, float v, const vec3& p) const
    {
        return emit-> value(u, v, p);
    }
    texture* emit;
};

//isotropic----------------------------------------------------------------------------------------------------------
class isotropic : public material
{
public:
    isotropic(texture* a) : albedo(a) {}
    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const
    {
        scattered = ray(rec.p, random_in_unit_sphere());
        attenuation = albedo->value(rec.u, rec.v, rec.p);
        return true;
    }    
    texture* albedo;
};
#endif