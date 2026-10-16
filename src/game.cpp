#include <kos.h>
#include <dc/pvr.h>
#include <kos/img.h>
#include <kmg/kmg.h>
#include <unistd.h>
#include <cstdio>
#include <cmath>
#include <vector>
#include <string>

alignas(8) pvr_poly_hdr_t texture;
pvr_ptr_t image = nullptr;
uint32_t image_w = 0, image_h = 0;

void init_image(std::string path, pvr_list_t mode, int format, pvr_filter_mode_t filter) {
    kos_img_t img;
    pvr_poly_cxt_t context;
    if (kmg_to_img(path.c_str(), &img) < 0) {
        dbglog(DBG_ERROR, "[ERROR] Failed to load: %s\n", path.c_str());
        return;
    }

    image = pvr_mem_malloc(img.w*img.h*2);
    pvr_txr_load_kimg(&img, image, PVR_TXRLOAD_16BPP);

    image_w = img.w;
    image_h = img.h;
    
    kos_img_free(&img, false);

    pvr_poly_cxt_txr(&context, mode, format, image_w, image_h, image, filter);
    pvr_poly_compile(&texture, &context);
}

void render_texture(const pvr_poly_hdr_t& source, int flags, float x, float y, std::vector<float> z, std::vector<float> u, std::vector<float> v, float width, float height, uint32_t color) {
    pvr_prim(&source, sizeof(source));

    pvr_vertex_t vert;
    
    vert.flags = flags;
    vert.argb = color;

    for (size_t i = 0; i < 4; i++) {
        if (i == 3) vert.flags = PVR_CMD_VERTEX_EOL;
        vert.x = (i % 2 == 0) ? x : x+width;
        vert.y = ((i / 2) % 2 == 0) ? y : y+height;
        vert.z = z[i];
        vert.u = u[i];
        vert.v = v[i];
        pvr_prim(&vert, sizeof(vert));
        /*
        dbglog(DBG_DEBUG, "[DEBUG] x=%f y=%f z=%f u=%f v=%f\n", vert.x, vert.y, vert.z, vert.u, vert.v);
        dbglog(DBG_DEBUG, "[DEBUG] header: ptr=%p size=%u\n", static_cast<const void*>(&source), static_cast<unsigned>(sizeof(source)));
        dbglog(DBG_DEBUG, "[DEBUG] vertex: ptr=%p size=%u\n", static_cast<const void*>(&vert), static_cast<unsigned>(sizeof(vert)));
        */
    }
}

int main() {
    pvr_init_defaults();

    std::vector<float> z_axis = {1.0f, 1.0f, 1.0f, 1.0f};
    std::vector<float> u_axis, v_axis;
   
    for (size_t i = 0; i < 4; i++) {
        u_axis.push_back((i % 2 == 0) ? 0.0f : 1.0f);
        v_axis.push_back(((i / 2) % 2 == 0) ? 0.0f : 1.0f);
    }

    init_image("/rd/background.kmg", PVR_LIST_OP_POLY, PVR_TXRFMT_RGB565 | PVR_TXRFMT_NONTWIDDLED, PVR_FILTER_NONE);

    if (!image) {
        dbglog(DBG_ERROR, "[ERROR] Image not loaded, exiting...");
        return -1;
    }

    while (true) {
        pvr_wait_ready();
        pvr_scene_begin();
        pvr_list_begin(PVR_LIST_OP_POLY);

        render_texture(texture, PVR_CMD_VERTEX, 0.0f, 0.0f, z_axis, u_axis, v_axis, 640.0f, 480.0f, 0xFFFFFFFF);

        pvr_list_finish();
        pvr_scene_finish();
    }
    return 0;
}