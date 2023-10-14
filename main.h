#ifndef MAIN_H
#define MAIN_H

using namespace std;
#include <cstdint>

#pragma pack(push, 1)
class BMPHeader {
private:
    uint16_t signature;
    uint32_t fileSize;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t dataOffset;
    uint32_t headerSize;
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t bitsPerPix;
    uint32_t compression;
    uint32_t imageSize;
    int32_t xPixelsPerMeter;
    int32_t yPixelsPerMeter;
    uint32_t totalColors;
    uint32_t importantColors;
public:
    uint16_t getSign(){
        return signature;
    }
    uint32_t getSize(){
        return fileSize;
    }
    uint32_t getDataOffset(){
        return dataOffset;
    }
    int32_t getWidth(){
        return width;
    }
    int32_t getHeight(){
        return height;
    }
    uint16_t getBitsPP(){
        return bitsPerPix;
    }
    void setHeight(int n){
        height = n;
    }
    void setWidth(int n){
        width = n;
    }
};
#pragma pack(pop)
#endif//MAIN_H