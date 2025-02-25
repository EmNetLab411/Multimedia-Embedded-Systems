#include <iostream>
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
#include <libswscale/swscale.h>
}

#define SPEED_UP_FACTOR 2  // Hệ số tua nhanh (2x)

// Hàm ghi frame vào bộ mã hóa
void encode_and_write(AVCodecContext* enc_ctx, AVFormatContext* out_fmt_ctx, AVStream* out_stream, AVFrame* frame) {
    if (avcodec_send_frame(enc_ctx, frame) == 0) {
        AVPacket* enc_pkt = av_packet_alloc();
        while (avcodec_receive_packet(enc_ctx, enc_pkt) == 0) {
            enc_pkt->stream_index = out_stream->index;
            av_interleaved_write_frame(out_fmt_ctx, enc_pkt);
            av_packet_unref(enc_pkt);
        }
        av_packet_free(&enc_pkt);
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <input_video> <output_video>" << std::endl;
        return -1;
    }

    const char* input_filename = argv[1];
    const char* output_filename = argv[2];

    avformat_network_init();

    // Mở file đầu vào
    AVFormatContext* input_fmt_ctx = nullptr;
    if (avformat_open_input(&input_fmt_ctx, input_filename, nullptr, nullptr) != 0) {
        std::cerr << "Error opening input file" << std::endl;
        return -1;
    }

    if (avformat_find_stream_info(input_fmt_ctx, nullptr) < 0) {
        std::cerr << "Error finding stream info" << std::endl;
        return -1;
    }

    // Tìm stream video
    const AVCodec* decoder = nullptr;
    int video_stream_index = av_find_best_stream(input_fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, &decoder, 0);
    if (video_stream_index < 0) {
        std::cerr << "Error finding video stream" << std::endl;
        return -1;
    }

    AVCodecContext* decoder_ctx = avcodec_alloc_context3(decoder);
    avcodec_parameters_to_context(decoder_ctx, input_fmt_ctx->streams[video_stream_index]->codecpar);
    avcodec_open2(decoder_ctx, decoder, nullptr);

    // Thiết lập file đầu ra
    AVFormatContext* output_fmt_ctx = nullptr;
    avformat_alloc_output_context2(&output_fmt_ctx, nullptr, nullptr, output_filename);
    const AVCodec* encoder = avcodec_find_encoder(AV_CODEC_ID_H264);
    AVCodecContext* encoder_ctx = avcodec_alloc_context3(encoder);

    encoder_ctx->width = decoder_ctx->width;
    encoder_ctx->height = decoder_ctx->height;
    encoder_ctx->pix_fmt = AV_PIX_FMT_YUV420P;
    encoder_ctx->time_base = (AVRational){1, 30};  // Ghi video với 30 FPS
    encoder_ctx->framerate = (AVRational){30, 1}; 
    avcodec_open2(encoder_ctx, encoder, nullptr);

    // Tạo stream đầu ra
    AVStream* out_stream = avformat_new_stream(output_fmt_ctx, nullptr);
    avcodec_parameters_from_context(out_stream->codecpar, encoder_ctx);
    avio_open(&output_fmt_ctx->pb, output_filename, AVIO_FLAG_WRITE);

    if (avformat_write_header(output_fmt_ctx, nullptr) < 0) {
        std::cerr << "Lỗi: Không thể ghi header vào output file!" << std::endl;
        return -1;
    }

    // Xử lý frame
    AVPacket* pkt = av_packet_alloc();
    int frame_count = 0;

    while (av_read_frame(input_fmt_ctx, pkt) >= 0) {
        if (pkt->stream_index == video_stream_index) {
            if (avcodec_send_packet(decoder_ctx, pkt) == 0) {
                AVFrame* frame = av_frame_alloc();
                while (avcodec_receive_frame(decoder_ctx, frame) == 0) {
                    if (frame_count % SPEED_UP_FACTOR == 0) {  // Giữ lại frame này
                        encode_and_write(encoder_ctx, output_fmt_ctx, out_stream, frame);
                    }
                    frame_count++;
                    av_frame_unref(frame);
                }
                av_frame_free(&frame);
            }
        }
        av_packet_unref(pkt);
    }

    // Ghi kết thúc file
    av_write_trailer(output_fmt_ctx);
    avformat_close_input(&input_fmt_ctx);
    avcodec_free_context(&decoder_ctx);
    avcodec_free_context(&encoder_ctx);
    avformat_free_context(output_fmt_ctx);
    av_packet_free(&pkt);
    avformat_network_deinit();

    std::cout << "✅ Fast-forwarded video saved as " << output_filename << std::endl;
    return 0;
}
