/*
	TEXGEN.h
*/

#include <stdlib.h>

#include "Misc.h"
#include "Texture.h"

class TexGen
{
private:
	// The pixel class
	class pixel
	{
	public:
		// Color components
		unsigned char r, g, b;

		// Default constructor
		pixel(int nr=0, int ng=0, int nb=0): r(nr), g(ng), b(nb) {}

		// Copy constructor
		pixel(const pixel &px): r(px.r), g(px.g), b(px.b) {}

		// Overloaded assignment operator
		pixel &operator=(const pixel &px)
		{
			r = px.r;
			g = px.g;
			b = px.b;

			return *this;
		}

		// Overloaded increment operator
		pixel &operator+=(const pixel &px)
		{
			r = clamp<int>(r + px.r, 0, 255);
			g = clamp<int>(g + px.g, 0, 255);
			b = clamp<int>(b + px.b, 0, 255);

			return *this;
		}

		// Overloaded scalar multiplication
		friend pixel operator*(const pixel &px, float s)
		{
			return pixel(clamp<int>((int)(px.r * s), 0, 255),
						 clamp<int>((int)(px.g * s), 0, 255),
						 clamp<int>((int)(px.b * s), 0, 255));
		}

		// Overloaded add operator
		friend pixel operator+(const pixel &px1, const pixel &px2)
		{
			return pixel(clamp<int>(px1.r + px2.r, 0, 255),
						 clamp<int>(px1.g + px2.g, 0, 255),
						 clamp<int>(px1.b + px2.b, 0, 255));
		}
	};

	// The layers
	pixel *layers[4];

	// Create filters
	void CreateFlt(unsigned char(*func)(int, int, const unsigned char *), const unsigned char *args);

	static unsigned char Clear(int x, int y, const unsigned char *args);
	static unsigned char Noise(int x, int y, const unsigned char *args);
	static unsigned char Environ(int x, int y, const unsigned char *args);

	// Color filters
	void ColorFlt(void(*func)(int, int, const unsigned char *, pixel *), const unsigned char *args);

	static void Bright(int x, int y, const unsigned char *args, pixel *data);
	static void Gradient(int x, int y, const unsigned char *args, pixel *data);
	static void Greyscale(int x, int y, const unsigned char *args, pixel *data);

	// Distort filters
	void DistFlt(void(*func)(int, int, float *, float *, const unsigned char *), const unsigned char *args);

	static void Offset(int x, int y, float *nx, float *ny, const unsigned char *args);
	static void SineDist(int x, int y, float *nx, float *ny, const unsigned char *args);

	// Layer transfer filters
	void TransFlt(void(*func)(pixel *, const pixel *), const unsigned char *args);
	static void TransAdd(pixel *px1, const pixel *px2);
	static void TransSub(pixel *px1, const pixel *px2);
	static void TransMul(pixel *px1, const pixel *px2);
	static void TransMin(pixel *px1, const pixel *px2);
	static void TransMax(pixel *px1, const pixel *px2);
	static void TransCopy(pixel *px1, const pixel *px2);

public:
	TexGen()
	{
		// Alloc memory
		layers[0] = new pixel[256*256];
		layers[1] = new pixel[256*256];
		layers[2] = new pixel[256*256];
		layers[3] = new pixel[256*256];
	}

	~TexGen()
	{
		// Free memory
		delete layers[0];
		delete layers[1];
		delete layers[2];
		delete layers[3];
	}

	Texture *Make(const unsigned char *data, bool allowMipMaps);
};

Texture *TexGen::Make(const unsigned char *data, bool allowMipMaps = true)
{
	const unsigned char *p = data;

	// Clear the layers
	ZeroMemory(layers[0], sizeof(pixel)*256*256);
	ZeroMemory(layers[1], sizeof(pixel)*256*256);
	ZeroMemory(layers[2], sizeof(pixel)*256*256);
	ZeroMemory(layers[3], sizeof(pixel)*256*256);

	// Get the number of filters
	int numFilters = *p++;

	while(numFilters--)
	{
		// The 8th argument is always a random seed
		srand(p[8]);

		// Apply the filter
		switch(*p++)
		{
		case 0x01:
			CreateFlt(Clear, p);
			break;
		case 0x02:
			CreateFlt(Noise, p);
			break;
		case 0x03:
			CreateFlt(Environ, p);
			break;
		case 0x04:
			/*CreateFlt(*/
			break;
		case 0x11:
			ColorFlt(Bright, p);
			break;
		case 0x12:
			ColorFlt(Gradient, p);
			break;
		case 0x13:
			ColorFlt(Greyscale, p);
			break;
		case 0x21:
			DistFlt(Offset, p);
			break;
		case 0x22:
			DistFlt(SineDist, p);
			break;
		case 0x31:
			TransFlt(TransAdd, p);
			break;
		case 0x32:
			TransFlt(TransSub, p);
			break;
		case 0x33:
			TransFlt(TransMul, p);
			break;
		case 0x34:
			TransFlt(TransMin, p);
			break;
		case 0x35:
			TransFlt(TransMax, p);
			break;
		case 0x36:
			TransFlt(TransCopy, p);
			break;
		default:
			break;
		};
		
		// Increase to the next filter
		p += 8;
	}

	// Make a texture from the contents of layer 0
	Texture::Image *img = new Texture::Image;
	img->width = img->height = 256;
	img->format = GL_RGB;
	img->data = new unsigned char [256*256*3];
	memcpy(img->data, layers[0], 256*256*3);

	// And return the pointer to the newly created texture object
	return new Texture(img, allowMipMaps);
}

void TexGen::CreateFlt(unsigned char(*func)(int, int, const unsigned char *), const unsigned char *args)
{
	// Which layer / color channel
	unsigned char *buf = (unsigned char *)layers[(*args) & 0xF0] + ((*args) & 0xF);

	// Run through all pixels and get the color values from the
	// specified filter
	for(int y = 0; y < 256; y++)
		for(int x = 0; x < 256; x++)
		{
			*buf = func(x, y, args);
			buf += 3;
		}
}

// Clears the layer to a specified color
unsigned char TexGen::Clear(int x, int y, const unsigned char *args)
{
	return args[1];
}

// Creates some random noise
unsigned char TexGen::Noise(int x, int y, const unsigned char *args)
{
	return rand()%0xff;
}

// Creates an environment map (brightness based on distance from center)
unsigned char TexGen::Environ(int x, int y, const unsigned char *args)
{
	float dist = sqrtf((float)((x-128)*(x-128) + (y-128)*(y-128)));
	int ax = 255-(int)(dist * 2 * (args[1]/255.0f));

	return clamp<int>(ax, 0, 255);
}

void TexGen::ColorFlt(void(*func)(int, int, const unsigned char *, pixel *), const unsigned char *args)
{
	// Which buffer
	pixel *buf = layers[(*args) & 0xF0];

	// Run the filter on all the pixels
	for(int y = 0; y < 256; y++)
		for(int x = 0; x < 256; x++)
		{
			func(x, y, args, buf++);
		}
}

// Adds to the pixel if args[1] > 127, otherwise it subtracts
void TexGen::Bright(int x, int y, const unsigned char *args, TexGen::pixel *data)
{
	int bias = args[1]*2 - 255;

	data->r = clamp(data->r + bias, 0, 255);
	data->g = clamp(data->g + bias, 0, 255);
	data->b = clamp(data->b + bias, 0, 255);
}

// Creates one of six preset gradients
void TexGen::Gradient(int x, int y, const unsigned char *args, TexGen::pixel *data)
{
	int sum = data->r + data->g + data->b;

	switch(args[1])
	{
	case 0: // Black - Red - Yellow - White
		data->r = clamp<int>(sum, 0, 255);
		data->g = clamp<int>(sum-255, 0, 255);
		data->b = clamp<int>(sum-510, 0, 255);
		break;
	case 1: // Black - Red - Purple - White
		data->r = clamp<int>(sum, 0, 255);
		data->b = clamp<int>(sum-255, 0, 255);
		data->g = clamp<int>(sum-510, 0, 255);
		break;
	case 2: // Black - Green - Yellow - White
		data->g = clamp<int>(sum, 0, 255);
		data->r = clamp<int>(sum-255, 0, 255);
		data->b = clamp<int>(sum-510, 0, 255);
		break;
	case 3: // Black - Green - Cyan - White
		data->g = clamp<int>(sum, 0, 255);
		data->b = clamp<int>(sum-255, 0, 255);
		data->r = clamp<int>(sum-510, 0, 255);
		break;
	case 4: // Black - Blue - Purple - White
		data->b = clamp<int>(sum, 0, 255);
		data->r = clamp<int>(sum-255, 0, 255);
		data->g = clamp<int>(sum-510, 0, 255);
		break;
	case 5: // Black - Blue - Cyan - White
		data->b = clamp<int>(sum, 0, 255);
		data->g = clamp<int>(sum-255, 0, 255);
		data->r = clamp<int>(sum-510, 0, 255);
		break;
	}
}

// Makes the texture gray
void TexGen::Greyscale(int x, int y, const unsigned char *args, TexGen::pixel *data)
{
	int light = (data->r + data->g + data->b) / 3;

	data->r = clamp(light, 0, 255);
	data->g = clamp(light, 0, 255);
	data->b = clamp(light, 0, 255);
}

#define mkxy(x1, y1) wrap(y1) * 256 + wrap(x1);

void TexGen::DistFlt(void(*func)(int, int, float *, float *, const unsigned char *), const unsigned char *args)
{
	// Temporary buffer
	static pixel buf1[256][256];

	// Which layer
	pixel *buf = layers[(*args) & 0xF0];

	// Copy it to the temp buffer
	memcpy(buf1, buf, sizeof(buf1));

	float dx, dy;
	float wx, wy;
	float t;
	int rx, ry;

	pixel tmpcol;

	for(int y = 0; y < 256; y++)
		for(int x = 0; x < 256; x++)
		{
			// Get the coords to grab the pixel from
			func(x, y, &dx, &dy, args);

			// Add 256 to avoid negative values
			dx += 256.0f;
			dy += 256.0f;

			// Get the whole pixel coordinates
			rx = (int)dx;
			ry = (int)dy;

			// Get the fractional part
			wx = dx - rx;
			wy = dy - ry;

			// Calculate the color based on the fractional
			// and the neighboring pixels
			t = (1-wx)*(1-wy);
			tmpcol = buf1[wrap(ry)][wrap(rx)] * t;
			t = wx*(1-wy);
			tmpcol += buf1[wrap(ry)][wrap(rx+1)] * t;
			t = (1-wx)*wy;
			tmpcol += buf1[wrap(ry+1)][wrap(rx)] * t;
			t = wx*wy;
			tmpcol += buf1[wrap(ry+1)][wrap(rx+1)] * t;

			// Put the color to the layer
			*buf++ = tmpcol;
		}

}

// Shifts the image
void TexGen::Offset(int x, int y, float *nx, float *ny, const unsigned char *args)
{
	*nx = (float)(x + args[1]);
	*ny = (float)(y + args[2]);
}

// Distorts the image with a sine function
void TexGen::SineDist(int x, int y, float *nx, float *ny, const unsigned char *args)
{
	*nx = (float)x + sinf(y*(PI/127.5f)*args[1] + args[5])*args[3];
	*ny = (float)y + sinf(x*(PI/127.5f)*args[2] + args[6])*args[4];
}

void TexGen::TransFlt(void(*func)(pixel *, const pixel *), const unsigned char *args)
{
	// Which buffers
	pixel *buf1 = layers[(*args++) & 0xF0];
	pixel *buf2 = layers[(*args++) & 0xF0];

	// Transfer pixels from one layer to the other
	int a = 256*256;
	while(a--)
		func(buf1++, buf2++);
}

// Adds the pixels
void TexGen::TransAdd(TexGen::pixel *px1, const TexGen::pixel *px2)
{
	px1->r = clamp<int>(px1->r + px2->r, 0, 255);
	px1->g = clamp<int>(px1->g + px2->g, 0, 255);
	px1->b = clamp<int>(px1->b + px2->b, 0, 255);
}

// Subtracts the second layer from the first
void TexGen::TransSub(TexGen::pixel *px1, const TexGen::pixel *px2)
{
	px1->r = clamp<int>(px1->r - px2->r, 0, 255);
	px1->g = clamp<int>(px1->g - px2->g, 0, 255);
	px1->b = clamp<int>(px1->b - px2->b, 0, 255);
}

// Multiplies the layers
void TexGen::TransMul(TexGen::pixel *px1, const TexGen::pixel *px2)
{
	px1->r = clamp<int>((px1->r * px2->r) / 255, 0, 255);
	px1->g = clamp<int>((px1->g * px2->g) / 255, 0, 255);
	px1->b = clamp<int>((px1->b * px2->b) / 255, 0, 255);
}

// Returns a pixel with the minimum color components
void TexGen::TransMin(TexGen::pixel *px1, const TexGen::pixel *px2)
{
	px1->r = clamp<int>(min<int>(px1->r, px2->r), 0, 255);
	px1->g = clamp<int>(min<int>(px1->g, px2->g), 0, 255);
	px1->b = clamp<int>(min<int>(px1->b, px2->b), 0, 255);
}

// Returns a pixel with the maximum color components
void TexGen::TransMax(TexGen::pixel *px1, const TexGen::pixel *px2)
{
	px1->r = clamp<int>(max<int>(px1->r, px2->r), 0, 255);
	px1->g = clamp<int>(max<int>(px1->g, px2->g), 0, 255);
	px1->b = clamp<int>(max<int>(px1->b, px2->b), 0, 255);
}

// Simply copies pixel2 to pixel1
void TexGen::TransCopy(TexGen::pixel *px1, const TexGen::pixel *px2)
{
	px1->r = px2->r;
	px1->g = px2->g;
	px1->b = px2->b;
}


