#include "ray.h"
#include "hitable_list.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>

vec3 color(const ray& r, hitable* world, int depth)
{
    hit_record rec;
    if(world -> hit(r, 0.001, 1e8, rec)) {
        ray scattered;
        vec3 attenuation;
        if(depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered))
        {
            return attenuation * color(scattered, world, depth + 1);
        } 
        else {
            return vec3(0);
        }
    }
    else {
        vec3 unit_direction = unit_vector(r.direction());
        float t = 0.5 * (unit_direction.y() + 1.0);
        return vec3(1.0 - t) + t * vec3(0.5, 0.7, 1.0);
    }
}

hitable* random_scene()
{
    int n = 500;
    hitable** list = new hitable*[n+1];
    list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(vec3(0.5))); //the plate
    int i = 1;
    for(int a = -11; a < 11; ++a) {
        for(int b = -11; b < 11; ++b)
        {
            float choose_mat = random();
            vec3 center(a + 0.9 * random(), 0.2, b + 0.9 * random());
            if((center - vec3(4, 0.2, 0)).length() > 0.9)
            {
                if(choose_mat < 0.8)
                {
                    list[i++] = new sphere(center, 0.2, new lambertian(vec3(random() * random(),
                                                                            random() * random(), 
                                                                            random() * random())));
                }
                else if (choose_mat < 0.95)
                {
                    list[i++] = new sphere(center, 0.2, 
                        new metal(vec3(0.5 * (1 + random()),
                                        0.5 * (1 + random()), 
                                        0.5 * (1 + random())), 
                                0.5 * (1 + random())));                    
                }
                else {
                    list[i++] = new sphere(center, 0.2, new dielectric(1.5));                    
                }
            }
        }
    }

    list[i++] = new sphere(vec3(0, 1, 0), 1.0, new dielectric(1.5));
    list[i++] = new sphere(vec3(-4, 1, 0), 1.0, new lambertian(vec3(0.4, 0.2, 0.1)));   
    list[i++] = new sphere(vec3(4, 1, 0), 1.0, new metal(vec3(0.7, 0.6, 0.5), 0));                    
                 
    return new hitable_list(list, i);
}

hitable* basic_scence()
{
    hitable** list = new hitable*[4];
    list[0] = new sphere(vec3(0, 0, -1), 0.5, new lambertian(vec3(0.1, 0.2, 0.5)));
    list[1] = new sphere(vec3(0, -100.5, -1), 100, new lambertian(vec3(0.8, 0.8, 0.0)));
    list[2] = new sphere(vec3(1, 0, -1), 0.5, new metal(vec3(0.8, 0.6, 0.2), 0.5));
    list[3] = new sphere(vec3(-1, 0, -1), 0.5, new dielectric(1.5));

    return new hitable_list(list, 4);
}

int main()
{
    int nx = 1280;
    int ny = 720;
    int ns = 100;
    std::string name = "random_100sample_720p";
    std::ofstream pic(name + ".ppm");
    pic << "P3\n" << nx << " " << ny << "\n255\n";

    hitable* world = random_scene();

    vec3 lookfrom(13, 2, 3);
    vec3 lookat(0, 0, 0);
    float dist_to_focus = 10.0;
    float aperture = 0.1;

/*
    vec3 lookfrom(3, 3, 2);
    vec3 lookat(0, 0, -1);
    float dist_to_focus = (lookfrom - lookat).length();
    float aperture = 2.0;
*/
    camera cam(lookfrom, lookat, vec3(0, 1, 0), 20, float(nx) / float(ny), aperture, dist_to_focus);

    srand((unsigned)time(NULL));
    
    for(int j = ny-1; j >= 0; --j)
        for(int i = 0; i < nx; ++i) {
            vec3 col(0);
            for(int s = 0; s < ns; ++s)
            {
                float u = float(i + random()) / float(nx);
                float v = float(j + random()) / float(ny);
                ray r = cam.get_ray(u, v);
                col += color(r, world, 0);
            }
            col /= float(ns);
            col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
            int ir = int(255.99 * col.e[0]);
            int ig = int(255.99 * col.e[1]);
            int ib = int(255.99 * col.e[2]);

            pic << ir << " " << ig << " " << ib << "\n";
        }
}