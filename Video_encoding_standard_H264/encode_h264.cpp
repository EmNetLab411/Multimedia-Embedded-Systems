#include <stdio.h>
#include <cstdint>  // Bổ sung cho uint8_t, uint16_t, uint32_t
#include <cstdlib>  // Bổ sung cho malloc, free
#include <stdlib.h>
#include <string.h>
#include <x264.h>

#define WIDTH  176
#define HEIGHT 144
#define FPS    30
#define FRAME_COUNT 300

int main() {
    FILE *yuv_file = fopen("input.yuv", "rb");
    FILE *h264_file = fopen("output.h264", "wb");
    if (!yuv_file || !h264_file) {
        perror("File open error");
        return -1;
    }

    x264_param_t param;
    x264_picture_t pic_in, pic_out;
    x264_t *encoder;
    x264_nal_t *nals;
    int i_nals;

    // Cấu hình bộ mã hóa
    x264_param_default_preset(&param, "veryfast", "zerolatency");
    param.i_csp = X264_CSP_I420;
    param.i_width = WIDTH;
    param.i_height = HEIGHT;
    param.i_fps_num = FPS;
    param.i_fps_den = 1;
    param.i_keyint_max = FPS * 2;
    param.b_intra_refresh = 1;
    param.rc.i_rc_method = X264_RC_CRF;
    param.rc.f_rf_constant = 23.0;
    x264_param_apply_profile(&param, "high");

    encoder = x264_encoder_open(&param);
    if (!encoder) {
        fprintf(stderr, "Failed to open x264 encoder\n");
        return -1;
    }

    x264_picture_alloc(&pic_in, X264_CSP_I420, WIDTH, HEIGHT);

    for (int i = 0; i < FRAME_COUNT; i++) {
        if (fread(pic_in.img.plane[0], 1, WIDTH * HEIGHT, yuv_file) <= 0 ||
            fread(pic_in.img.plane[1], 1, WIDTH * HEIGHT / 4, yuv_file) <= 0 ||
            fread(pic_in.img.plane[2], 1, WIDTH * HEIGHT / 4, yuv_file) <= 0) {
            break;
        }
        pic_in.i_pts = i;

        if (x264_encoder_encode(encoder, &nals, &i_nals, &pic_in, &pic_out) >= 0) {
            for (int j = 0; j < i_nals; j++) {
                fwrite(nals[j].p_payload, 1, nals[j].i_payload, h264_file);
            }
        }
    }

    x264_picture_clean(&pic_in);
    x264_encoder_close(encoder);
    fclose(yuv_file);
    fclose(h264_file);

    printf("Encoding completed. Output saved to output.h264\n");
    return 0;
}