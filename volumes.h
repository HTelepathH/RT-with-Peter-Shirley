#ifndef VOLUMES_H
#define VOLUMES_H

#include "hitable.h"
#include "material.h"
#include <float.h>

class constant_medium : public hitable
{
public:
    constant_medium(hitable* b, float d, texture* a) : boundary(b), density(d)
    {
        phase_function = new isotropic(a);
    }
    virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const
    {
        hit_record rec1, rec2;
        if(boundary->hit(r, -FLT_MAX, FLT_MAX, rec1))
        {
            if(boundary->hit(r, rec1.t + 0.0001, FLT_MAX, rec2))
            {
                rec1.t = fmax(rec1.t, t_min);
                rec2.t = fmin(rec2.t, t_max);
                if(rec1.t >= rec2.t) return false;
                rec1.t = fmax(0, rec1.t);
                float distance_inside_boundary = (rec2.t - rec1.t) * r.direction().length();
                float hit_distance = -(1 / density) * log(random());
                if(hit_distance < distance_inside_boundary)
                {
                    rec.t = rec1.t + hit_distance / r.direction().length();
                    rec.p = r.point_at_parameter(rec.t);
                    rec.normal = vec3(1, 0, 0);
                    rec.mat_ptr = phase_function;
                    return true;
                }
            }
        }
        return false;
    }
    virtual bool bounding_box(float t0, float t1, aabb& box) const
    {
        return boundary->bounding_box(t0, t1, box);
    }
    hitable* boundary;
    float density;
    material* phase_function;
};

#endif