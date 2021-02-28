#ifndef __LANGUAGE3D_H_
#define __LANGUAGE3D_H_



#include "raytraCer/objects.h"
#include "raytraCer/camera.h"
#include "raytraCer/renderer.h"

#include "core.h"









List* gisp_make_group3D(List* a);
List* gisp_make_material_lambertian(List* a);
List* gisp_make_material_light(List* a);
List* gisp_make_material_metal(List* a);
List* gisp_make_material_dielectric(List* a);

List* gisp_add_to_group3D(List* a);
List* gisp_flip_face3D(List* a);
List* gisp_rect3D(List* a);
List* gisp_box3D(List* a);
List* gisp_sphere3D(List* a);
List* gisp_rotated3D(List* a);
List* gisp_translated3D(List* a);

List* gisp_camera3D(List* a);
List* gisp_render3D(List* a);





#endif // __LANGUAGE3D_H_
