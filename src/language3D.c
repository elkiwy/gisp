#include "language3D.h"







float3 gispTaggedVectorToFloat3(void* vector){
    Vector* p = (Vector*)untag_vector(vector);
    void** d = p->data;
    return (float3){numVal(d[0]),numVal(d[1]),numVal(d[2])};
}



///=Gisp Core: 3D

///~Creates a container group for 3D objects
///&make-group3D
///#Group3D
__attribute__((aligned(16))) List* gisp_make_group3D(List* a){
    ObjectList* group = make_objectList(1024);
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
__attribute__((aligned(16))) List* gisp_make_material_lambertian(List* a){
    double r = numVal(first(a));
    double g = numVal(second(a));
    double b = numVal(third(a));
    Texture* tex = make_texture_solid(r, g, b);
    Material* mat = make_material_lambertian(tex);
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
__attribute__((aligned(16))) List* gisp_make_material_light(List* a){
    double r = numVal(first(a));
    double g = numVal(second(a));
    double b = numVal(third(a));
    Texture* tex = make_texture_solid(r, g, b);
    Material* mat = make_material_light(tex);
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
__attribute__((aligned(16))) List* gisp_make_material_metal(List* a){
    double r = numVal(first(a));
    double g = numVal(second(a));
    double b = numVal(third(a));
    double fuzz = numVal(fourth(a));
    Texture* tex = make_texture_solid(r, g, b);
    Material* mat = make_material_metal((float)fuzz, tex);
    return (List*)mat;
}

///~Creates a dielectric material for 3D objects
///&make-material-dielectric
///#Material3D
///!1Number
///@1r
__attribute__((aligned(16))) List* gisp_make_material_dielectric(List* a){
    double ir = numVal(first(a));
    Material* mat = make_material_dielectric((float)ir);
    return (List*)mat;
}

///~Append a new 3D objects to the group
///&add-to-group3D
///#Group3D
///!1Group3D
///@1group
///!2Object3D
///@2obj
__attribute__((aligned(16))) List* gisp_add_to_group3D(List* a){
    ObjectList* group = first(a);
    Object* obj = second(a);
    printf("Adding %p to %p\n", obj, group);fflush(stdout);
    add_object_to_objectList(obj, group);
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
__attribute__((aligned(16))) List* gisp_rect3D(List* a){
    float3 p1 = gispTaggedVectorToFloat3(first(a));
    float p1x = p1.x;
    float p1y = p1.y;
    float p1z = p1.z;
    float3 p2 = gispTaggedVectorToFloat3(second(a));
    float p2x = p2.x;
    float p2y = p2.y;
    float p2z = p2.z;

    Material* mat = (Material*)third(a);

    axis rect_a;
    double k;
    float c00, c01, c10, c11;
    if (p1x == p2x){
        k = p1x; rect_a = AXIS_YZ;
        c00 = p1y; c01 = p1z;
        c10 = p2y; c11 = p2z;
    }else if (p1y == p2y){
        k = p1y; rect_a = AXIS_XZ;
        c00 = p1x; c01 = p1z;
        c10 = p2x; c11 = p2z;
    }else if (p1z == p2z){
        k = p1z; rect_a = AXIS_XY;
        c00 = p1x; c01 = p1y;
        c10 = p2x; c11 = p2y;
    }else{printf("\nERROR: rect3D is not axis aligned, please align on one axis.\n\n");fflush(stdout);
        return e_nil;
    }


    return (List*)make_rect(c00,c01, c10,c11, k, rect_a, mat);
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
__attribute__((aligned(16))) List* gisp_box3D(List* a){
    float3 min = gispTaggedVectorToFloat3(first(a));
    float minx = min.x;
    float miny = min.y;
    float minz = min.z;
    float3 max = gispTaggedVectorToFloat3(second(a));
    float maxx = max.x;
    float maxy = max.y;
    float maxz = max.z;
    Material* mat = (Material*)third(a);
    return (List*)make_box(minx, miny, minz, maxx, maxy, maxz, mat);
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
__attribute__((aligned(16))) List* gisp_sphere3D(List* a){
    float3 c = gispTaggedVectorToFloat3(first(a));
    double radius = numVal(second(a));
    Material* mat = (Material*)third(a);
    return (List*)make_sphere(c.x, c.y, c.z, radius, mat);
}




///~Wrap around a 3D object flipping its face normals
///&flip-face
///#Object3D
///!1Object3D
///@1obj
__attribute__((aligned(16))) List* gisp_flip_face3D(List* a){
    Object* obj = first(a);
    return (List*)make_flip_face(obj);
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
__attribute__((aligned(16))) List* gisp_rotated3D(List* a){
    Object* obj = first(a);
    double angle = numVal(second(a));
    int ax = (int)(numVal(third(a)));
    rot_axis rot_a;
    if (ax==0){rot_a = AXIS_X;}
    else if (ax==1){rot_a = AXIS_Y;}
    else {rot_a = AXIS_Z;}
    return (List*)make_rotated(obj, rot_a, angle);
}

///~Wrap around a 3D object translating it
///&translated3D
///#Object3D
///!1Object3D
///@1obj
///!2Point3D / Number[3]
///@2offset
__attribute__((aligned(16))) List* gisp_translated3D(List* a){
    Object* obj = first(a);
    float3 offset = gispTaggedVectorToFloat3(second(a));
    return (List*)make_translated(obj, offset.x, offset.y, offset.z);
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
__attribute__((aligned(16))) List* gisp_camera3D(List* a){
    float3 lookfrom = gispTaggedVectorToFloat3(first(a));
    float3 lookat = gispTaggedVectorToFloat3(second(a));
    float3 vup = {0,1,0};
    double vfov = numVal(third(a));
    double dist_to_focus = numVal(fourth(a));
    double aperture = numVal(fifth(a));
    float ASPECT_RATIO = 1.0;

    camera* c = make_camera(lookfrom, lookat, vup, vfov, 1.0, aperture, dist_to_focus);
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
__attribute__((aligned(16))) List* gisp_render3D(List* a){
    char* filename = (char*)untag_string(first(a));
    int IMAGE_WIDTH = (int)numVal(second(a));
    int IMAGE_HEIGHT = (int)numVal(third(a));
    double samples_per_pixel = numVal(fourth(a));
    ObjectList* world = fifth(a);
    ObjectList* lights = car(cdr(cdr(cdr(cdr(cdr(a))))));
    camera* cam = car(cdr(cdr(cdr(cdr(cdr(cdr(a)))))));
    int max_recursion_depth = 8;
    float3 background = {0,0,0};

    printf("params done\n");fflush(stdout);

    printf("world %p lights %p\n", world, lights);fflush(stdout);

    scene* s = make_scene(world, lights, 128);
    printf("scene done\n");fflush(stdout);

    scene_settings settings;
    settings.res_width         = IMAGE_WIDTH;
    settings.res_height        = IMAGE_HEIGHT;
    settings.samples_per_pixel = samples_per_pixel;
    settings.iterations        = max_recursion_depth;

    printf("settings done\n");fflush(stdout);
    render_scene(s, cam, &settings, filename);

    printf("render done\n");fflush(stdout);
    free_camera(cam);
    free_scene(s);
    printf("free done\n");fflush(stdout);

    return e_nil;
}
