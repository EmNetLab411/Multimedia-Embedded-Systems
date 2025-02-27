#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

// Ghi 1 byte vào file
void writeByte(ofstream &file, uint8_t value) {
    file.write(reinterpret_cast<char*>(&value), sizeof(value));
}

// Ghi 2 byte (Big Endian) vào file
void write2Bytes(ofstream &file, uint16_t value) {
    writeByte(file, (value >> 8) & 0xFF);
    writeByte(file, value & 0xFF);
}

// Ghi 4 byte (Big Endian) vào file
void write4Bytes(ofstream &file, uint32_t value) {
    writeByte(file, (value >> 24) & 0xFF);
    writeByte(file, (value >> 16) & 0xFF);
    writeByte(file, (value >> 8) & 0xFF);
    writeByte(file, value & 0xFF);
}

// Ghi chuỗi ký tự
void writeString(ofstream &file, const string &str) {
    file.write(str.c_str(), str.size());
}

// Ghi sự kiện MIDI (Note On, Note Off)
void writeMidiEvent(ofstream &file, uint8_t status, uint8_t data1, uint8_t data2) {
    writeByte(file, status);
    writeByte(file, data1);
    writeByte(file, data2);
}

// Ghi độ trễ (Delta Time) cho sự kiện
void writeDeltaTime(ofstream &file, uint32_t delta) {
    if (delta < 0x80) {
        writeByte(file, delta);
    } else {
        writeByte(file, (delta >> 7) | 0x80);
        writeByte(file, delta & 0x7F);
    }
}

// Ghi giai điệu với các nốt nhạc và độ dài mong muốn
void writeMelody(vector<uint8_t> &trackData, vector<int> melody, int repeat) {
    for (int r = 0; r < repeat; ++r) {
        for (int i = 0; i < melody.size(); ++i) {
            // Note On
            trackData.push_back(0x00);      // Delta time
            trackData.push_back(0x90);      // Note On (Channel 0)
            trackData.push_back(melody[i]); // Nốt nhạc
            trackData.push_back(0x64);      // Velocity (âm lượng)

            // Note Off
            trackData.push_back(0x81);      // Delta time (khoảng 1 beat)
            trackData.push_back(0x00);
            trackData.push_back(0x80);      // Note Off (Channel 0)
            trackData.push_back(melody[i]); // Nốt nhạc
            trackData.push_back(0x40);      // Velocity
        }
    }
}

int main() {
    ofstream file("output.mid", ios::binary);

    // Header Chunk
    writeString(file, "MThd");          // Chunk Type
    write4Bytes(file, 6);               // Chunk Length
    write2Bytes(file, 0);               // Format Type 0 (1 track)
    write2Bytes(file, 1);               // Số lượng track
    write2Bytes(file, 480);             // Ticks per beat

    // Track Chunk
    writeString(file, "MTrk");          // Chunk Type (Track)
    vector<uint8_t> trackData;

    // Tempo (120 BPM)
    trackData.push_back(0x00);          // Delta time
    trackData.push_back(0xFF);          // Meta event
    trackData.push_back(0x51);          // Set Tempo
    trackData.push_back(0x03);          // Length
    trackData.push_back(0x07);
    trackData.push_back(0xA1);
    trackData.push_back(0x20);

    // Giai điệu chính (nốt nhạc của Despacito)
    vector<int> melody = {64, 66, 68, 69, 71, 73, 75, 76};  // Giai điệu đơn giản hóa

    // Ghi giai điệu chính và lặp lại nhiều lần để dài hơn
    writeMelody(trackData, melody, 4);

    // Thêm một octave cao hơn cho biến tấu
    for (int &note : melody) note += 12;
    writeMelody(trackData, melody, 4);

    // Thêm một octave thấp hơn cho đoạn kết
    for (int &note : melody) note -= 24;
    writeMelody(trackData, melody, 4);

    // End of Track
    trackData.push_back(0x00);          // Delta time
    trackData.push_back(0xFF);          // Meta event
    trackData.push_back(0x2F);          // End of Track
    trackData.push_back(0x00);          // Length

    // Ghi Track Chunk vào file
    write4Bytes(file, trackData.size());
    for (uint8_t byte : trackData) {
        writeByte(file, byte);
    }

    file.close();
    cout << "Create output.mid success!" << endl;

    // Tự động phát nhạc sau khi tạo xong
    system("timidity output.mid");

    return 0;
}

