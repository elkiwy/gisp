#ifndef __LANGUAGE3D_H_
#define __LANGUAGE3D_H_


#include <omp.h>


#include "raytraCer/hittable_list.h"
#include "raytraCer/camera.h"

//#include "raytraCer/extern_stb_image_write.h"
#include "raytraCer/external/stb_image_write.h"

#include "core.h"


List* make_group3D(List* a);
List* make_material_lambertian(List* a);
List* make_material_light(List* a);
List* make_material_metal(List* a);
List* make_material_dielectric(List* a);

List* add_to_group3D(List* a);
List* flip_face3D(List* a);
List* rect3D(List* a);
List* box3D(List* a);
List* sphere3D(List* a);
List* rotated3D(List* a);
List* translated3D(List* a);

List* camera3D(List* a);
List* render3D(List* a);





#endif // __LANGUAGE3D_H_
