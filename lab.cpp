#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include "main.h"

using namespace std;
 /* Программа хорошо ложится на использование класса картинки. То есть
 * у нас мог бы быть класс BMP, который содержит все эти методы,
 * в приватных полях хранит указатель на пиксели и BMPHeader. При 
 * изменении картинки создает новый экземпляр картинки. Тогда мы бы 
 * имели более безопасную программу за счет инкапсуляции. */

 /* Но использование константных ссылок к правильным объектам радует, молодец */
void rotateClock(const vector<uint8_t>& inputData, vector<uint8_t>& outputData, int32_t width, int32_t height) {
    outputData.resize(inputData.size());
    /* Тут уже необязательно использовать int32_t, просто int*/
    for (int32_t x = 0; x < width; ++x) {
        for (int32_t y = 0; y < height; ++y) {
            for (int8_t chan = 0; chan < 3; ++chan)
            outputData[(x * height + y) * 3 + chan] = inputData[(y * width + (width - 1 - x)) * 3 + chan];
        }
    }
}

void rotateCounterclock(const vector<uint8_t>& inputData, vector<uint8_t>& outputData, int32_t width, int32_t height) {
    outputData.resize(inputData.size());
    for (int32_t x = 0; x < width; ++x) {
        for (int32_t y = 0; y < height; ++y) {
            for (int8_t chan = 0; chan < 3; ++chan)
            outputData[(x * height + y) * 3 + chan] = inputData[((height - 1 - y) * width + x) * 3 + chan];
        }
    }
}

void GaussFilter(const vector<uint8_t>& inputData, vector<uint8_t>& outputData, int32_t width, int32_t height) {
    outputData.resize(inputData.size());
     /* Получилась довольно сильная вложенность циклов. Такое следует разбивать, возможно с использованием
    * вспомогательной функции. И вероятно, у нее будет много параметров. Однако нужно стремиться к тому,
    * чтобы у функции было не больше 5 параметров. Так что какие-то придется объединить в одну структуру */
    for (int32_t i = 1; i < height - 1; ++i) {
        for (int32_t j = 1; j < width - 1; ++j) {
            for (int k = 0; k < 3; ++k) {
                int32_t sum = 0;
                for (int y = -1; y <= 1; ++y) {
                    for (int x = -1; x <= 1; ++x) {
                        int32_t index = ((i + y) * width + (j + x)) * 3 + k;
                        sum += inputData[index];
                    }
                }
                outputData[(i * width + j) * 3 + k] = static_cast<uint8_t>(sum / 9);
            }
        }
    }
}

int main(int argc, char* argv[]) {
    ifstream inputFile("test3.bmp", ios::binary);
    if (!inputFile) {
        cerr << "Не удалось открыть входной файл." << endl;
        return 1;
    }

    // создание заголовков
    BMPHeader header;
    /* А если 100 картинок скажу прочитать, 100 раз скопируешь? */
    inputFile.read(reinterpret_cast<char*>(&header), sizeof(BMPHeader));
    BMPHeader outHeader = header;
    int newHeight = header.getWidth();
    int newWidth = header.getHeight();
    outHeader.setHeight(newHeight);
    outHeader.setWidth(newWidth);

    // проверка поддержки файла
    if (header.getSign() != 0x4D42 || header.getBitsPP() != 24) {
        cerr << "Формат файла не поддерживается." << endl;
        return 1;
    }
    /* Между заголовком и пикселями остались данные, которые ты не прочитала. 
    * Поэтому на моем компьютере твоя программа вовсе не работает. Эти данные
    * следовало считать, чтобы потом без изменений записать. А то потеряли*/
    // Считываем данные изображения
    vector<uint8_t> imageData(header.getSize() - header.getDataOffset());
    inputFile.seekg(header.getDataOffset(), ios::beg);
    inputFile.read(reinterpret_cast<char*>(imageData.data()), imageData.size());

    //поворот на 90 по часовой
    vector<uint8_t> rotatedImageClock;

    rotateClock(imageData, rotatedImageClock, header.getWidth(), header.getHeight());

    ofstream outputClock("out1.bmp", ios::binary);
    if (!outputClock) {
        cerr << "Не удалось создать выходной файл." << endl;
        return 1;
    }
    outputClock.write(reinterpret_cast<char*>(&outHeader), sizeof(BMPHeader));
    outputClock.write(reinterpret_cast<char*>(rotatedImageClock.data()), rotatedImageClock.size());

    //поворот на 90 против часовой
    vector<uint8_t> rotatedImageCounterclock;
    rotateCounterclock(imageData, rotatedImageCounterclock, header.getWidth(), header.getHeight());

    ofstream outputCounterclock("out2.bmp", ios::binary);
    if (!outputCounterclock) {
        cerr << "Не удалось создать выходной файл." << endl;
        return 1;
    }
    outputCounterclock.write(reinterpret_cast<char*>(&outHeader), sizeof(BMPHeader));
    outputCounterclock.write(reinterpret_cast<char*>(rotatedImageCounterclock.data()), rotatedImageCounterclock.size());

    //фильтр гаусса
    vector<uint8_t> gaussImage;
    GaussFilter(rotatedImageCounterclock, gaussImage, header.getHeight(), header.getWidth());

    ofstream outputGaussImage("out3.bmp", ios::binary);
    if (!outputGaussImage) {
        cerr << "Не удалось создать выходной файл." << endl;
        return 1;
    }
    outputGaussImage.write(reinterpret_cast<char*>(&outHeader), sizeof(BMPHeader));
    outputGaussImage.write(reinterpret_cast<char*>(gaussImage.data()), gaussImage.size());

    return 0;
}
