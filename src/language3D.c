#include "language3D.h"





vec3 gispTaggedVectorToVec3(void* vector){
    Vector* p = (Vector*)untag_vector(vector);
    void** d = p->data;
    return (vec3){numVal(d[0]),numVal(d[1]),numVal(d[2])};
}



///=Gisp Core: 3D

///~Creates a container group for 3D objects
///&make-group3D
///#Group3D
__attribute__((aligned(16))) List* make_group3D(List* a){
    hittable_list* group = hittable_list_new(1024);
    printf("Made group3D: %p\n", group);
    return (List*)group;
}

///~Creates a lambertian material for 3D objects
///&make-material-lambertian
///#Material3D
///!1Number
///@1r
///!2Number
///@2g
///!3Number
///@3b
__attribute__((aligned(16))) List* make_material_lambertian(List* a){
    double r = numVal(first(a));
    double g = numVal(second(a));
    double b = numVal(third(a));
    material* mat = material_lambertian_new_from_color((color){r, g, b});
    return (List*)mat;
}

///~Creates a light material for 3D objects
///&make-material-light
///#Material3D
///!1Number
///@1r
///!2Number
///@2g
///!3Number
///@3b
__attribute__((aligned(16))) List* make_material_light(List* a){
    double r = numVal(first(a));
    double g = numVal(second(a));
    double b = numVal(third(a));
    material* mat = material_light_new_from_color((color){r, g, b});
    return (List*)mat;
}

///~Creates a metal material for 3D objects
///&make-material-metal
///#Material3D
///!1Number
///@1r
///!2Number
///@2g
///!3Number
///@3b
///!4Number
///@4fuzzy
__attribute__((aligned(16))) List* make_material_metal(List* a){
    double r = numVal(first(a));
    double g = numVal(second(a));
    double b = numVal(third(a));
    double fuzz = numVal(fourth(a));
    material* mat = material_metal_new((color){r,g,b}, fuzz);
    return (List*)mat;
}

///~Creates a dielectric material for 3D objects
///&make-material-dielectric
///#Material3D
///!1Number
///@1r
__attribute__((aligned(16))) List* make_material_dielectric(List* a){
    double ir = numVal(first(a));
    material* mat = material_dielectric_new(ir);
    return (List*)mat;
}

///~Append a new 3D objects to the group
///&add-to-group3D
///#Group3D
///!1Group3D
///@1group
///!2Object3D
///@2obj
__attribute__((aligned(16))) List* add_to_group3D(List* a){
    hittable_list* group = first(a);
    hittable* obj = second(a);
    hittable_list_add(group, obj);
    return (List*)group;
}

///~Creates an axis aligned rectangle
///&rect3D
///#Object3D
///!1Point3D / Number[3]
///@1p1
///!2Point3D / Number[3]
///@2p2
///!3Material3D
///@3mat
__attribute__((aligned(16))) List* rect3D(List* a){
    vec3 p1 = gispTaggedVectorToVec3(first(a));
    double p1x = p1.x;
    double p1y = p1.y;
    double p1z = p1.z;
    vec3 p2 = gispTaggedVectorToVec3(second(a));
    double p2x = p2.x;
    double p2y = p2.y;
    double p2z = p2.z;

    material* mat = (material*)third(a);

    rect_axis axis;
    double k;
    if (p1x == p2x) {      k = p1x; axis = YZ;
    }else if (p1y == p2y){ k = p1y; axis = XZ;
    }else if (p1z == p2z){ k = p1z; axis = XY;
    }else{printf("\nERROR: rect3D is not axis aligned, please align on one axis.\n\n");fflush(stdout);
        return e_nil;
    }

    return (List*)hittable_rect_new(NULL, p1x,p2x, p1y,p2y, p1z,p2z, k, axis, mat);
}


///~Creates an 3D box
///&box3D
///#Object3D
///!1Point3D / Number[3]
///@1p1
///!2Point3D / Number[3]
///@2p2
///!3Material3D
///@3mat
__attribute__((aligned(16))) List* box3D(List* a){
    vec3 min = gispTaggedVectorToVec3(first(a));
    vec3 max = gispTaggedVectorToVec3(second(a));
    material* mat = (material*)third(a);
    return (List*)hittable_box_new(NULL, min, max, mat);
}


///~Creates an 3D sphere
///&sphere3D
///#Object3D
///!1Point3D / Number[3]
///@1center
///!2Number
///@2r
///!3Material3D
///@3mat
__attribute__((aligned(16))) List* sphere3D(List* a){
    vec3 c = gispTaggedVectorToVec3(first(a));
    double radius = numVal(second(a));
    material* mat = (material*)third(a);
    return (List*)hittable_sphere_new(NULL, c, radius, mat);
}




///~Wrap around a 3D object flipping its face normals
///&flip-face
///#Object3D
///!1Object3D
///@1obj
__attribute__((aligned(16))) List* flip_face3D(List* a){
    hittable* obj = first(a);
    return (List*)hittable_flip_face_init(NULL, obj);
}

///~Wrap around a 3D object rotating it
///&rotated3D
///#Object3D
///!1Object3D
///@1obj
///!2Number
///@2angle
///!3Number
///@3axis
__attribute__((aligned(16))) List* rotated3D(List* a){
    hittable* obj = first(a);
    double angle = numVal(second(a));
    int ax = (int)(numVal(third(a)));
    rotation_axis axis;
    if (ax==0){axis = X;}
    else if (ax==1){axis = Y;}
    else if (ax==2){axis = Z;}
    return (List*)hittable_rotate_init(NULL, obj, angle, axis);
}

///~Wrap around a 3D object translating it
///&translated3D
///#Object3D
///!1Object3D
///@1obj
///!2Point3D / Number[3]
///@2offset
__attribute__((aligned(16))) List* translated3D(List* a){
    hittable* obj = first(a);
    vec3 offset = gispTaggedVectorToVec3(second(a));
    return (List*)hittable_translate_init(NULL, obj, offset);
}














///~Creates a 3D camera object to render a 3D scene
///&camera3D
///#Camera3D
///!1Point3D / Number[3]
///@1origin
///!2Point3D / Number[3]
///@2lookAt
///!3Number
///@3fov
///!4Number
///@4focus
///!5Number
///@5aperture
__attribute__((aligned(16))) List* camera3D(List* a){
    vec3 lookfrom = gispTaggedVectorToVec3(first(a));
    vec3 lookat = gispTaggedVectorToVec3(second(a));
    printf("look from:\n");
    vec3_print(&lookfrom);
    printf("look at:\n");
    vec3_print(&lookat);
    vec3 vup = {0,1,0};
    double vfov = numVal(third(a));
    double dist_to_focus = numVal(fourth(a));
    double aperture = numVal(fifth(a));
    double ASPECT_RATIO = 1.0;
    camera* c = camera_new(lookfrom, lookat, vup, vfov, ASPECT_RATIO, aperture, dist_to_focus, 0.0, 1.0);
    printf("Made camera: %p\n", c);
    return (List*)c;
}

///~Render a 3D scene to a file
///&render3D
///#void
///!1String
///@1filename
///!2Number
///@2sw
///!3Number
///@3sh
///!4Number
///@4spp
///!5Group3D
///@5scenegroup
///!6Group3D
///@6lightsgroup
///!7Camera3D
///@7camera
__attribute__((aligned(16))) List* render3D(List* a){
    char* filename = (char*)untag_string(first(a));
    int IMAGE_WIDTH = (int)numVal(second(a));
    int IMAGE_HEIGHT = (int)numVal(third(a));
    double samples_per_pixel = numVal(fourth(a));
    hittable_list* world = fifth(a);
    hittable_list* lights = car(cdr(cdr(cdr(cdr(cdr(a))))));
    camera* cam = car(cdr(cdr(cdr(cdr(cdr(cdr(a)))))));
    int max_recursion_depth = 32;

    color background = {0,0,0};

    //PNG output setup
    const int CHANNELS = 3;
    unsigned char pixels[IMAGE_WIDTH * IMAGE_HEIGHT * CHANNELS];
    for (int i=0;i<IMAGE_WIDTH*IMAGE_HEIGHT;i++){
        pixels[i*3] = 255;
        pixels[i*3+1] = 0;
        pixels[i*3+2] = 0;
    }

    //Setup profiling and multithreading
    double time = 0.0;
    double begin = omp_get_wtime();
    int progress = 0;
    const unsigned int THREADS = 4*4;
    int step = floor(IMAGE_HEIGHT/THREADS);
    int chunksDone = 0;

    //Split height into chunks and give them to threads
    //#pragma omp parallel for
    for(unsigned int k=0; k<THREADS; ++k){
        double thread_begin = omp_get_wtime();

        //Each threads cycle a step numer of rows
        for(int sj=step-1; sj>=0; --sj){
            int j = sj+(k*step);
            //Cycle pixel inside each row
            for(int i=0; i<IMAGE_WIDTH; ++i){
                //Sample the same pixel with variations on the casted ray to create antialiasing
                color pixel_color = {0,0,0};
                for(int s=0; s<samples_per_pixel; ++s){
                    //Cast ray and sum the color
                    double u = ((double)i+random_double()) / (double)(IMAGE_WIDTH - 1);
                    double v = ((double)j+random_double()) / (double)(IMAGE_HEIGHT - 1);
                    ray r = camera_get_ray(cam, u, v);
                    pixel_color = vec3c_sum(pixel_color, ray_color(&r, &background, world, lights, max_recursion_depth));
                    //pixel_color = (vec3){1,0,0};
                }

                //Output the result color into the file
                int index = (i+(j*IMAGE_WIDTH)) * CHANNELS;

                write_color(pixels, &pixel_color, samples_per_pixel, index);
            }
        }

        //Output feedback of chunk completed
        int thread_id = omp_get_thread_num();
        double thread_end = omp_get_wtime();
        chunksDone++;
        printf("Thread %d (chunk: %d-%d) finished in %f, remains %d chunks.\n", thread_id, k*step, (k+1)*step, (double)(thread_end - thread_begin), THREADS-chunksDone);fflush(stdout);
    }

    //Flip the final image and save it
    stbi_flip_vertically_on_write(1);
    stbi_write_png(filename, IMAGE_WIDTH, IMAGE_HEIGHT, CHANNELS, pixels, IMAGE_WIDTH * CHANNELS);

    //Output total time elapsed
    double end = omp_get_wtime();
    time = (double)(end - begin);
    printf("Time elpased for rendering %f\n", time);


    return e_nil;
}
