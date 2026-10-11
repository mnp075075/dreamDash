#include <kos.h>
#include <dc/pvr.h>
#include <kos/img.h>
#include <kmg/kmg.h> 
#include <iostream>
#include <string>

pvr_poly_hdr_t color_poly_hdr;
pvr_ptr_t image;
uint32_t image_w = 0, image_h = 0;

void init_shape(pvr_list_t type) {
    pvr_poly_cxt_t poly_cxt;
    pvr_poly_cxt_col(&poly_cxt, type);
    pvr_poly_compile(&color_poly_hdr, &poly_cxt);
}

void draw_image(std::string path) {
    kos_img_t img;
    if (kmg_to_img(path.c_str(), &img) < 0) {
        dbglog(DBG_ERROR, "[ERROR] Failed to load: %s\n",
           path.c_str());
        return;
    }

    image = pvr_mem_malloc(img.w*img.h*2);
    pvr_txr_load_kimg(&img, image, PVR_TXRLOAD_16BPP);

    image_w = img.w;
    image_h = img.h;
    
    kos_img_free(&img, false);
}

void draw_rectangle(float x, float y, float width, float height, uint32_t color) {
    pvr_wait_ready();
    pvr_scene_begin();

    pvr_list_begin(PVR_LIST_OP_POLY);

    pvr_prim(&color_poly_hdr, sizeof(color_poly_hdr));

    pvr_vertex_t vert;
    
    vert.flags = PVR_CMD_VERTEX;
    vert.x = x;
    vert.y = y;
    vert.z = 1.0f;
    vert.argb = color;
    pvr_prim(&vert, sizeof(vert));

    vert.x = x + width;
    vert.y = y;
    vert.z = 1.0f;
    pvr_prim(&vert, sizeof(vert));

    vert.x = x;
    vert.y = y + height;
    vert.z = 1.0f;
    pvr_prim(&vert, sizeof(vert));

    vert.flags = PVR_CMD_VERTEX_EOL;
    vert.x = x + width;
    vert.y = y + height;
    vert.z = 1.0f;
    pvr_prim(&vert, sizeof(vert));

    pvr_list_finish();
    pvr_scene_finish();
}

int main() {
    pvr_init_defaults();
    draw_image("/romdisk/background.kmg");
    while (true) {
        pvr_wait_ready();
        pvr_scene_begin();

        pvr_list_begin(PVR_LIST_OP_POLY);
        pvr_list_finish();

        pvr_scene_finish();
    }
}