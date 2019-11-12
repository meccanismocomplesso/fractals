/*
	TEXTURE.h
*/

#ifndef _TEXTURE_
#define _TEXTURE_

class Texture
{
public:
	// Structure to hold a texture image
	struct Image
	{
		int width, height;
		unsigned int format;
		unsigned char *data;
	};

	// Creates a texture from an image (deletes the image afterwards)
	Texture(Image *img, bool allowMipMaps = true)
	{
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);

		switch(prefs::texquality)
		{
		case 0:	// No interpolation
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			break;
		case 1:	// Linear interpolation
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			break;
		case 2:	// Linear interpolation w/mipmaps
			if(allowMipMaps)
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			}
			else
			{
				// Special fx may require non-mipmapped textures, even if mipmapping is desired
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			}
			break;
		};

		// Set texture wrap mode to repeat
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// Upload the texture
		if(prefs::texquality == 2 && allowMipMaps)
			gluBuild2DMipmaps(GL_TEXTURE_2D, img->format, img->width, img->height, img->format, GL_UNSIGNED_BYTE, img->data);
		else 
			glTexImage2D(GL_TEXTURE_2D, 0, img->format, img->width, img->height, 0, img->format, GL_UNSIGNED_BYTE, img->data);

		// Free memory
		delete [] img->data;
		delete img;
	}

	// Frees memory
	~Texture()
	{
		glDeleteTextures(1, &id);
	}

	// Binds the texture
	void Bind()
	{
		glBindTexture(GL_TEXTURE_2D, id);
	}

	// Makes the texture clamp properly
	//void ClampToEdge()
	//{
	//	glBindTexture(GL_TEXTURE_2D, id);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//}

	// Enable/disable texturing
	static void Enable(bool yes)
	{
		if(yes)
			glEnable(GL_TEXTURE_2D);
		else
			glDisable(GL_TEXTURE_2D);
	}
private:
	// The texture id
	unsigned int id;
};


#endif // _TEXTURE_
