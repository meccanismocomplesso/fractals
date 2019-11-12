/*
	FONT.h
*/

#ifndef _FONT_
#define _FONT_

#include <stdio.h>

class Font
{
private:
	float x, y;
	float coords[256][4][2];
	int widths[256];
	int height;
	Texture *tex;

public:
	// Creates a font
	Font(const char *fontname, int fontsize, bool bold, bool italic)
	{
		int x, y;
		int texsize = 512;
		int padding = italic?4:2; // Italic fonts require more padding

		// Create a memory device context
		HDC hDC = CreateCompatibleDC(GetDC(0));

		if(!hDC)
			throw("Font failed to create memory DC");

		// And a memory bitmal
		BITMAPINFO bmpi = {0};
		bmpi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmpi.bmiHeader.biWidth = texsize;
		bmpi.bmiHeader.biHeight = texsize;
		bmpi.bmiHeader.biPlanes = 1;
		bmpi.bmiHeader.biBitCount = 32;
		bmpi.bmiHeader.biCompression = BI_RGB;

		// Doing it this way gives me direct access to the bitmap memory
		BYTE *pBits = NULL;
		HBITMAP hBmp = CreateDIBSection(hDC, &bmpi, DIB_RGB_COLORS, (void**)&pBits, NULL, 0);

		if(!hBmp)
		{
			DeleteObject(hDC);
			throw("Font failed to create memory DIB section");
		}

		SelectObject(hDC, hBmp);

		SetBkColor(hDC, RGB(0, 0, 0));
		SetTextColor(hDC, RGB(255, 255, 255));

		// Create the font
		HFONT fnt = CreateFont(fontsize, 0, 0, 0, bold?FW_HEAVY:FW_NORMAL, italic, false, false,
							   ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
							   FF_DONTCARE | DEFAULT_PITCH, fontname);

		
		if(!fnt)
		{
			DeleteObject(hBmp);
			DeleteObject(hDC);
			throw("Font failed to create truetype");
		}

		SelectObject(hDC, fnt);

		// Get character width information
		if(!GetCharWidth32(hDC, 0, 255, widths))
		{
			if(!GetCharWidth(hDC, 0, 255, widths))
			{
				DeleteObject(hBmp);
				DeleteObject(hDC);
				throw("Font failed to get character widths");
			}
		}

		// Render the characters to the bitmap
		unsigned char c = 0;
		int c_height = (int)(fontsize * 1.333f);
		height = c_height;
				
		for(y = c_height; (y < texsize) && (c < 256); y += c_height)
			for(x = widths[c]; (x < texsize) && (c < 256); x += widths[c] + padding)
			{
				TextOut(hDC, x-widths[c], y-c_height, (char *)&c, 1);
				c++;
			}

		// Make sure it's done
		GdiFlush();

		// Create a texture image from the bitmap
		Texture::Image *img = new Texture::Image;
		img->width = img->height = texsize;
		img->format = GL_RGBA;
		img->data = new unsigned char [img->width * img->height * 4];
		memcpy(img->data, pBits, img->width * img->height * 4);

		// Create a basic alpha channel
		for(int a = 0; a < img->width * img->height * 4; a += 4)
			img->data[a+3] = img->data[a];

		// Free some memory
		DeleteObject(fnt);
		DeleteObject(hBmp);
		DeleteObject(hDC);

		// Create the texture
		tex = new Texture(img);

		// Create the texture coordinates
		float u1, v1, u2, v2;

		c = 0;
		for(y = c_height; (y < texsize) && (c < 256); y += c_height)
			for(x = widths[c]; (x < texsize) && (c < 256); x += widths[c] + padding)
			{
				u1 = (x-widths[c])/(float)texsize;
				v1 = 1 - (y-c_height)/(float)texsize;
				u2 = x/(float)texsize;
				v2 = 1- y/(float)texsize;

				coords[c][0][0] = u1;
				coords[c][0][1] = v1;

				coords[c][1][0] = u2;
				coords[c][1][1] = v1;
				
				coords[c][2][0] = u2;
				coords[c][2][1] = v2;
				
				coords[c][3][0] = u1;
				coords[c][3][1] = v2;

				c++;
			}
	}

	// Free memory
	~Font()
	{
		delete tex;
	}

	// Prints text at the current position
	void Print(const char *fmt, ...)
	{
		va_list ap;
		
		unsigned char buffer[256];

		// Allow formatted output
		va_start(ap, fmt);
		vsprintf((char*)buffer, fmt, ap);
		va_end(ap);

		int l = strlen((char*)buffer);

		tex->Bind();

		// Draw the text
		glBegin(GL_QUADS);
			for(int a = 0; a < l; a++)
			{
				unsigned char c = buffer[a];

				glTexCoord2fv(coords[c][0]); glVertex2f(x, y + height);
				glTexCoord2fv(coords[c][1]); glVertex2f(x + widths[c], y + height);
				glTexCoord2fv(coords[c][2]); glVertex2f(x + widths[c], y);
				glTexCoord2fv(coords[c][3]); glVertex2f(x, y);

				x += widths[c];
			}
		glEnd();
	}

	// Prints text at a specified position
	void Print(float x1, float y1, const char *fmt, ...)
	{
		va_list ap;
		
		x = x1;
		y = y1;

		unsigned char buffer[256];

		// Allow formatted output
		va_start(ap, fmt);
		vsprintf((char*)buffer, fmt, ap);
		va_end(ap);

		int l = strlen((char*)buffer);

		tex->Bind();

		// Draw the text
		glBegin(GL_QUADS);
			for(int a = 0; a < l; a++)
			{
				unsigned char c = buffer[a];

				glTexCoord2fv(coords[c][0]); glVertex2f(x, y + height);
				glTexCoord2fv(coords[c][1]); glVertex2f(x + widths[c], y + height);
				glTexCoord2fv(coords[c][2]); glVertex2f(x + widths[c], y);
				glTexCoord2fv(coords[c][3]); glVertex2f(x, y);

				x += widths[c];
			}
		glEnd();
	}

	
};

#endif // _FONT_
