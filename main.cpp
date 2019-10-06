#include "ray.h"
#include "hitable_list.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"
#include "bvh.h"
#include "aabb.h"
#include "rectangle.h"
#include "box.h"
#include "instance.h"
#include "volumes.h"
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <float.h>

#define STB_IMAGE_IMPLEMENTATION
#include "../stb/stb_image.h"

vec3 color(const ray& r, hitable* world, int depth)
{
    hit_record rec;
    if(world -> hit(r, 0.001, FLT_MAX, rec)) {
        ray scattered;
        vec3 attenuation;
        vec3 emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);
        if(depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered))
        {
            return emitted + attenuation * color(scattered, world, depth + 1);
        } 
        else {
            return emitted;
        }
    }
    else {
        /*
        vec3 unit_direction = unit_vector(r.direction());
        float t = 0.5 * (unit_direction.y() + 1.0);
        return vec3(1.0 - t) + t * vec3(0.5, 0.7, 1.0);
        */
       return vec3(0);
    }
}

hitable* basic_scene()
{
    hitable** list = new hitable*[4];
    list[0] = new sphere(vec3(0, 0, -1), 0.5, 
                        new lambertian(new constant_texture(vec3(0.1, 0.2, 0.5))));
    list[1] = new sphere(vec3(0, -100.5, -1), 100, 
                        new lambertian(new constant_texture(vec3(0.8, 0.8, 0.0))));
    list[2] = new sphere(vec3(1, 0, -1), 0.5, new metal(vec3(0.8, 0.6, 0.2), 0.5));
    list[3] = new sphere(vec3(-1, 0, -1), 0.5, new dielectric(1.5));

    return new hitable_list(list, 4);
}

hitable* moving_scene()
{
    hitable** list = new hitable*[4];
    vec3 center(0, 0, -1);
    list[0] = new moving_sphere(center, center + vec3(0, 0.3, 0), 0.0, 1.0, 0.2, 
                new lambertian(new constant_texture(vec3(0.1, 0.2, 0.5))));
    list[1] = new sphere(vec3(0, -100.5, -1), 100, 
                new lambertian(new constant_texture(vec3(0.8, 0.8, 0.0))));
    list[2] = new sphere(vec3(1, 0, -1), 0.5, new metal(vec3(0.8, 0.6, 0.2), 0.5));
    list[3] = new sphere(vec3(-1, 0, -1), 0.5, new dielectric(1.5));

    return new hitable_list(list, 4);
}

hitable* random_scene()
{
    int n = 500;
    hitable** list = new hitable*[n+1];

    texture* checker = new checker_texture(new constant_texture(vec3(0.2, 0.3, 0.1)),
                                        new constant_texture(vec3(0.9, 0.9, 0.9)));
    list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(checker)); //the plate

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
                    list[i++] = new moving_sphere(center, center+vec3(0, 0.5 * random(), 0), 0.0, 1.0, 0.2, 
                                    new lambertian(new constant_texture(vec3(random() * random(),
                                                                            random() * random(), 
                                                                            random() * random()))));
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
    list[i++] = new sphere(vec3(-4, 1, 0), 1.0, new lambertian(new constant_texture(vec3(0.4, 0.2, 0.1))));   
    list[i++] = new sphere(vec3(4, 1, 0), 1.0, new metal(vec3(0.7, 0.6, 0.5), 0));                    
                 
    return new bvh_node(list, i, 0, 1);
}

hitable* two_spheres()
{
    texture* checker = new checker_texture(new constant_texture(vec3(0.2, 0.3, 0.1)),
                                        new constant_texture(vec3(0.9, 0.9, 0.9)));
    hitable** list = new hitable*[2];
    list[0] = new sphere(vec3(0, -10, 0), 10, new lambertian( checker));
    list[1] = new sphere(vec3(0, 10, 0), 10, new lambertian( checker));

    return new hitable_list(list, 2);
}

hitable* perlin_two_spheres()
{
    texture* pertex = new noise_texture(4);
    hitable** list = new hitable*[2];
    list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian( pertex));
    list[1] = new sphere(vec3(0, 2, 0), 2, new lambertian( pertex));
    return new hitable_list(list, 2);
}

hitable* image_texture_sphere()
{
    int nx, ny, nn;
    unsigned char* tex_data = stbi_load("texture/wall_albedo.png", &nx, &ny, &nn, 0);
    material* mat = new lambertian(new image_texture(tex_data, nx, ny));

    hitable** list = new hitable*[2];
    list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(new constant_texture(vec3(0.8, 0.8, 0.0))));
    list[1] = new sphere(vec3(0, 1, 0), 1, mat);
    return new hitable_list(list, 2);
}

hitable* simple_light()
{
    texture* pertex = new noise_texture(4);
    hitable** list = new hitable*[4];
    list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian( pertex));
    list[1] = new sphere(vec3(0, 2, 0), 2, new lambertian( pertex));
    list[2] = new sphere(vec3(0, 7, 0), 2, new diffuse_light(new constant_texture(vec3(4))));
    list[3] = new xy_rect(3, 5, 1, 3, -2, new diffuse_light(new constant_texture(vec3(4))));
    return new hitable_list(list, 4);
}

hitable* cornell_box()
{
    hitable** list = new hitable*[6];

    material* red = new lambertian(new constant_texture(vec3(0.65, 0.05, 0.05)));
    material* white = new lambertian(new constant_texture(vec3(0.73)));
    material* green = new lambertian(new constant_texture(vec3(0.12, 0.45, 0.15)));
    material* light = new diffuse_light(new constant_texture(vec3(15)));
    int i = 0;
    list[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
    list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
    list[i++] = new xz_rect(213, 343, 227, 332, 554, light);
    list[i++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
    list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
    list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));

    list[i++] = new translate(new rotate_y(new box(vec3(0), vec3(165), white), -18), vec3(130, 0, 65));
    list[i++] = new translate(new rotate_y(new box(vec3(0), vec3(165, 330, 165), white), 15), vec3(265, 0, 295));

    return new hitable_list(list, i);
}

hitable* cornell_smoke()
{
    hitable** list = new hitable*[8];

    material* red = new lambertian(new constant_texture(vec3(0.65, 0.05, 0.05)));
    material* white = new lambertian(new constant_texture(vec3(0.73)));
    material* green = new lambertian(new constant_texture(vec3(0.12, 0.45, 0.15)));
    material* light = new diffuse_light(new constant_texture(vec3(7)));
    int i = 0;
    list[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
    list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
    list[i++] = new xz_rect(113, 443, 127, 432, 554, light);
    list[i++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
    list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
    list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));

    hitable* b1 = new translate(new rotate_y(new box(vec3(0), vec3(165), white), -18), vec3(130, 0, 65));
    hitable* b2 = new translate(new rotate_y(new box(vec3(0), vec3(165, 330, 165), white), 15), vec3(265, 0, 295));

    list[i++] = new constant_medium(b1, 0.01, new constant_texture(vec3(1)));
    list[i++] = new constant_medium(b2, 0.01, new constant_texture(vec3(0)));

    return new hitable_list(list, i);
}

hitable* final()
{
    int nb = 20;
    hitable** list = new hitable*[30];
    hitable** boxlist = new hitable*[10000]; // floor
    hitable** boxlist2 = new hitable*[10000]; // bubble box
    material* white = new lambertian(new constant_texture(vec3(0.73)));
    material* ground = new lambertian(new constant_texture(vec3(0.48, 0.83, 0.53)));

    //floor
    int b = 0;
    for(int i = 0; i < nb; ++i)
        for(int j = 0; j < nb; ++j)
        {
            float w = 100;
            float x0 = -1000 + i * w;
            float z0 = -1000 + j * w;
            float y0 = 0;
            
            float x1 = x0 + w;
            float y1 = 100 * (random() + 0.01);
            float z1 = z0 + w;
            boxlist[b++] = new box(vec3(x0, y0, z0), vec3(x1, y1, z1), ground);
        }
    int l = 0;
    list[l++] = new bvh_node(boxlist, b, 0, 1);

    //light
    material* light = new diffuse_light(new constant_texture(vec3(7)));
    list[l++] = new xz_rect(123, 423, 147, 412, 554, light);

    //spheres metal / moving / dieletric
    vec3 center(400, 400, 200);
    list[l++] = new moving_sphere(center, center + vec3(30, 0, 0), 0, 1, 50, new lambertian(new constant_texture(vec3(0.7, 0.3, 0.1))));
    list[l++] = new sphere(vec3(260, 150, 45), 50, new dielectric(1.5));
    list[l++] = new sphere(vec3(0, 150, 145), 50, new metal(vec3(0.8, 0.8, 0.9), 10));
    hitable* boundary = new sphere(vec3(360, 150, 145), 70, new dielectric(1.5));
    list[l++] = boundary;

    //smoke in glass
    list[l++] = new constant_medium(boundary, 0.2, new constant_texture(vec3(0.2, 0.4, 0.9)));

    //fog
    boundary = new sphere(vec3(0), 5000, new dielectric(1.5));
    list[l++] = new constant_medium(boundary, 0.0001, new constant_texture(vec3(1)));

    //texture spheres
    int nx, ny, nn;
    unsigned char* tex_data = stbi_load("texture/wall_albedo.png", &nx, &ny, &nn, 0);
    material* wall_mat = new lambertian(new image_texture(tex_data, nx, ny));
    list[l++] = new sphere(vec3(400, 200, 400), 100, wall_mat);
    texture* pertex = new noise_texture(0.1);
    list[l++] = new sphere(vec3(220, 280, 300), 80, new lambertian(pertex));
    
    //bubble box
    int ns = 100;
    for(int j = 0; j < ns; ++j)
        boxlist2[j] = new sphere(vec3(165 * random(), 165 * random(), 165 * random()), 10, white);
    list[l++] = new translate(new rotate_y(new bvh_node(boxlist2, ns, 0, 1), 15), vec3(-100, 270, 395));

    
    return new hitable_list(list, l);
}

int main()
{
    int nx = 720;
    int ny = 720;
    int ns = 30;
    std::string name = "final2";
    std::ofstream pic(name + ".ppm");
    pic << "P3\n" << nx << " " << ny << "\n255\n";

/*
    vec3 lookfrom(3, 3, 2);
    vec3 lookat(0, 0, -1);
    float dist_to_focus = (lookfrom - lookat).length();
    float aperture = 0.2;
*/
/*
    hitable* world = perlin_two_spheres();
    vec3 lookfrom(13, 2, 3);
    vec3 lookat(0, 0, 0);
    float dist_to_focus = 10.0;
    float aperture = 0.0;
    float vfov = 20;
*/

    hitable* world = final();
    vec3 lookfrom(478, 278, -600);
    vec3 lookat(278,278,0);
    float dist_to_focus = 10.0;
    float aperture = 0.0;
    float vfov = 40.0;
/* 
    hitable* world = cornell_box();
    vec3 lookfrom(278, 278, -800);
    vec3 lookat(278, 278, 0);
    float dist_to_focus = 10.0;
    float aperture = 0.0;
    float vfov = 40.0;
*/
    camera cam(lookfrom, lookat, vec3(0, 1, 0), vfov, float(nx) / float(ny), aperture, dist_to_focus, 0, 1);

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
            col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2])); //gamma correction

            int ir = int(255.99 * col.e[0]) > 255 ? 255 : int(255.99 * col.e[0]);
            int ig = int(255.99 * col.e[1]) > 255 ? 255 : int(255.99 * col.e[1]);
            int ib = int(255.99 * col.e[2]) > 255 ? 255 : int(255.99 * col.e[2]);

            pic << ir << " " << ig << " " << ib << "\n";
        }

    std::cout << "The running time is:" << (double)clock() /CLOCKS_PER_SEC<< "s" << std::endl;
}