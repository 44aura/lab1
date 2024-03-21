#include "BMP.h"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>

int Image::Reader(const char* fileName){
    FILE* inputFile = fopen(fileName, "rb");
    if (!inputFile) {
        std::cerr << "Не удалось открыть входной файл." << std::endl;
        return 0;
    }
    fread(&header, sizeof(char), sizeof(BMPHeader), inputFile);

	int padding = (4 - header.width * 3 % 4) % 4;

    unsigned char* data = new unsigned char[(3 * header.width + padding) * header.height];
    fread(data, sizeof(char), (3 * header.width + padding) * header.height, inputFile);

    fclose(inputFile);
    imageData = data;
    std::cout << header.fileSize << " bytes = file size\n";
    return 0;
}

int Image::Writer(const char* fileName){
    FILE* outputFile = fopen(fileName, "wb");
    if (!outputFile) {
        std::cerr << "Не удалось открыть выходной файл." << std::endl;
        return 0;
    }
    fwrite(&header, sizeof(char), sizeof(BMPHeader), outputFile);

    int padding = (4 - header.width * 3 % 4) % 4;
    fwrite(imageData, sizeof(char), (3 * header.width + padding) * header.height, outputFile);
    fclose(outputFile);
    std::cout << "File written successfully\n";
    return 0;
}

Image Image::RotateClock() {
    BMPHeader newHeader = header;

    std::swap(newHeader.width, newHeader.height);
    int h = newHeader.height, w = newHeader.width;
    int curr_padding = (4 - 3 * w % 4) % 4, source_padding = (4 - 3 * h % 4) % 4;
    unsigned char* data = new unsigned char[(3 * w + curr_padding) * h];

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int start_pos = (x * h + h - 1 - y) * 3 + x * source_padding;
			int arrival_pos = (y * w + x) * 3 + y * curr_padding;
			std::copy_n(imageData + start_pos, 3, data + arrival_pos);
        }
    }
    return Image(newHeader, data);
}

Image Image::RotateCounterclock() {
    BMPHeader newHeader = header;

    std::swap(newHeader.width, newHeader.height);
    int h = newHeader.height, w = newHeader.width;
    int curr_padding = (4 - 3 * w % 4) % 4, source_padding = (4 - 3 * h % 4) % 4;
    unsigned char* data = new unsigned char[(3 * w + curr_padding) * h];

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int start_pos = ((w - 1 - x) * h + y) * 3 + (w - 1 - x) * source_padding;
			int arrival_pos = (y * w + x) * 3 + y * curr_padding;
			std::copy_n(imageData + start_pos, 3, data + arrival_pos);
        }
    }
    return Image(newHeader, data);
} 

Image Image::Apply_gaussian_blur(int radius)
{
	int padding = (4 - 3 * header.width % 4) % 4;
	std::vector<double> Gaus_Kernel = Get_matrix(radius);

	unsigned char* data = new unsigned char[(3 * header.width + padding) * header.height];
	for (int y = 0; y < header.height; y++){
		for (int x = 0; x < header.width; x++){
			int pixel_pos = (y * header.width + x) * 3 + y * padding;
			double* mean_colour = Get_gaus_colours(Gaus_Kernel, x, y, pixel_pos, padding);
			std::copy_n(mean_colour, 3, data + pixel_pos);
		}
	}
	return Image(header, data);
}

std::vector<double> Image::Get_matrix(int radius)
{
	const int matrix_size = 2 * radius + 1;
	const double pi = 3.14159, sigma = radius / 3;
	double sum = 0.0;

	std::vector<double> Gaus_Kernel(matrix_size * matrix_size, 0.0);
	for (int y = -radius; y <= radius; y++)
	{
		for (int x = -radius; x <= radius; x++)
		{
			Gaus_Kernel[(y + radius) * matrix_size + x + radius] = exp(-(x * x + y * y) / (2 * sigma * sigma)) / (2 * pi * sigma * sigma);
			sum += Gaus_Kernel[(y + radius) * matrix_size + x + radius];
		}
	}

	for (int y = 0; y < matrix_size; y++)
	{
		for (int x = 0; x < matrix_size; x++)
		{
			Gaus_Kernel[y * matrix_size + x] /= sum;
		}
	}

	return Gaus_Kernel;
}

double* Image::Get_gaus_colours(std::vector<double> Gaus_Kernel, int x, int y, int pixel_pos, int padding)
{
	int matrix_size = (int)sqrt(Gaus_Kernel.size()), radius = (matrix_size - 1) / 2;
	double* mean_colour = new double[3];
	for (int shift_h = 0; shift_h < matrix_size; shift_h++)
	{
		for (int shift_w = 0; shift_w < matrix_size; shift_w++)
		{
			int matrix_shift = 0;
			bool element_out_of_range = (y + shift_h - radius < 0 or y + shift_h - radius >= header.height or x + shift_w - radius < 0 or x + shift_w - radius >= header.width);
			if (!element_out_of_range)
			{
				matrix_shift = ((shift_h - radius) * header.width + shift_w - radius) * 3 + padding * (shift_h - radius);
			}
			mean_colour[0] += Gaus_Kernel[shift_h * matrix_size + shift_w] * imageData[pixel_pos + matrix_shift + 0];
			mean_colour[1] += Gaus_Kernel[shift_h * matrix_size + shift_w] * imageData[pixel_pos + matrix_shift + 1];
			mean_colour[2] += Gaus_Kernel[shift_h * matrix_size + shift_w] * imageData[pixel_pos + matrix_shift + 2];
		}
	}
	return mean_colour;
}