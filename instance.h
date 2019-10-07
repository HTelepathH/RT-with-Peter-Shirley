#ifndef INSTANCE_H
#define INSTANCE_H

#include "hitable.h"
#include "ray.h"

class translate : public hitable
{
public:
    translate(hitable* p, const vec3& displacement) : ptr(p), offset(displacement) {}
    virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const
    {
        ray moved_r(r.origin() - offset, r.direction(), r.time());
        if(ptr->hit(moved_r, t_min, t_max, rec))
        {
            rec.p += offset;
            return true;
        }else return false;
    }
    virtual bool bounding_box(float t0, float t1, aabb& box) const
    {
        if(ptr->bounding_box(t0, t1, box))
        {
            box = aabb(box.min() + offset, box.max() + offset);
            return true;
        }else return false;
    }
    hitable* ptr;
    vec3 offset;
};

class rotate_y : public hitable
{
public:
    rotate_y(hitable* p, float angle) : ptr(p)
    {
        float radians = (M_PI / 180.0) * angle;
        sin_theta = sin(radians);
        cos_theta = cos(radians);
        hasbox = ptr->bounding_box(0, 1, bbox);
        vec3 min(FLT_MAX), max(-FLT_MAX);
        for(int i = 0; i < 2; ++i)
            for(int j = 0; j < 2; ++j)
                for(int k = 0; k < 2; ++k)
                {
                    float x = i * bbox.max().x() + (1 - i) * bbox.min().x();
                    float y = j * bbox.max().y() + (1 - j) * bbox.min().y();
                    float z = k * bbox.max().z() + (1 - k) * bbox.min().z();
                    float newx = cos_theta * x + sin_theta * z;
                    float newz = -sin_theta * x + cos_theta * z;
                    vec3 tester(newx, y, newz);
                    for(int c = 0; c < 3; ++c)
                    {
                        if(tester[c] > max[c])
                            max[c] = tester[c];
                        if(tester[c] < min[c])
                            min[c] = tester[c];
                    }
                }
        
        bbox = aabb(min, max);
    }
    virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const
    {
        vec3 origin = inverse_rotate_around_y(r.origin());
        vec3 direction =  inverse_rotate_around_y(r.direction());
        ray rotated_r(origin, direction, r.time());

        if(ptr->hit(rotated_r, t_min, t_max, rec))
        {
            rec.p = rotate_around_y(rec.p);
            rec.normal = rotate_around_y(rec.normal);
            return true;
        }else return false;
    }
    virtual bool bounding_box(float t0, float t1, aabb& box) const
    {
        box = bbox;
        return true;
    }
    hitable* ptr;
    float sin_theta, cos_theta;
    bool hasbox;
    aabb bbox;
private:
    vec3 rotate_around_y(const vec3& v) const
    {
        return vec3( cos_theta * v[0] + sin_theta * v[2], v[1],
                    -sin_theta * v[0] + cos_theta * v[2]);
    }
    vec3 inverse_rotate_around_y(const vec3& v) const
    {
        return vec3( cos_theta * v[0] - sin_theta * v[2], v[1],
                     sin_theta * v[0] + cos_theta * v[2]);        
    }
};

#endif