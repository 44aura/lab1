#include <vector>
#include <stdint.h>

#pragma pack(push, 1)
struct BMPHeader 
{
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
    uint32_t unused[23];
};
#pragma pack(pop)

struct Image 
{
	BMPHeader header;
	unsigned char* imageData;

	Image(BMPHeader fheader, unsigned char* pixel_data)
	{
		header = fheader;
		imageData = pixel_data;
	}
	Image() {}

	int Reader(const char* filename);
	int Writer(const char* filename);
	Image RotateCounterclock();
	Image RotateClock();
	Image Apply_gaussian_blur(int radius);
	std::vector<double> Get_matrix(int radius);

	double* Get_gaus_colours(std::vector<double> Gaus_Kernel, int x, int y, int pixel_pos, int padding);

	~Image() 
	{
		delete[] imageData;
	}
};