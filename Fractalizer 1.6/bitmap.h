#ifndef BITMAP_H
#define BITMAP_H

#include <windows.h>
#include <stdio.h>
#include <time.h>

typedef struct tagRGBTriplet
{
	BYTE red;
	BYTE green;
	BYTE blue;
} RGBTriplet;


COLORREF drawRGB(int ii, int jj)
{
	float f = 1/zoom;
	a = (a1 + a2 * ii /sh)*f;
       	b = (b1 + b2 * jj /sw)*f;
	x = a;
	y = b;


	if(alg == MANDELINV)
	{
		float def = 1 / (a*a + b*b);
		a = a * def;
		b = -b * def;
		x = a;
		y = b;
	}

	if(alg == JULIA || alg == JULIA_CUBIC)
	{
		a = 0.42;
		b = 0.23;	
	}
	
	if(alg == JULIAN)
	{
		a = w;
		b = v;
		w = 1;
		v = 1;	
	}

	for(c=0; c < NC; c++)
	{
		float denom = 0;
		switch(alg)
		{
			case JULIA_CUBIC:
				xnew = w * x * x * x - 3 * v * x * y * y + a;
				ynew = - w * y * y * y + 3 * v * x * x * y + b;
				break;
			case NEWTON_RAPHSON:
				denom = w*3*((x*x - y*y)*(x*x - y*y) + 4*x*x*y*y);
				if (denom == 0) denom = .0000001;
	 			xnew = k*0.6666667*x + (x*x - y*y)/denom;
	 			ynew = v*0.6666667*y - 2*x*y/denom;
				break;
			case NEWTON_RAPHSON2:
				denom = w*4*((x*x*x - 3*x*y*y)*(x*x*x - 3*x*y*y) + (-y*y*y+3*x*x*y)*(-y*y*y+3*x*x*y));
				if (denom == 0) denom = .0000001;
	 			xnew = k*0.75*x + (x*x*x - 3*x*y*y)/denom;
	 			ynew = v*0.75*y - (-y*y*y + 3*x*x*y)/denom;
				break;
			case NEWTON_RAPHSON3:
				denom = w*5*((x*x*x*x + y*y*y*y - 6*x*x*y*y)*(x*x*x*x + y*y*y*y - 6*x*x*y*y) + 16*(x*x*x*y-x*y*y*y)*(x*x*x*y-x*y*y*y));
				if (denom == 0) denom = .0000001;
	 			xnew = k*0.8*x + (x*x*x*x + y*y*y*y - 6*x*x*y*y)/denom;
	 			ynew = v*0.8*y - 4*(x*x*x*y - x*y*y*y)/denom;
				break;
			default:
				xnew = w * x * x - v * y * y + a;
				ynew = 2 * k * x * y + b;
				break;
		}

		if( xnew == x && ynew == y){
			if(alg != NEWTON_RAPHSON)
				if(alg != NEWTON_RAPHSON2)
					if(alg != NEWTON_RAPHSON3)
					c = NC;
			break;
		}
		x = xnew;
		y = ynew;
		if(alg != NEWTON_RAPHSON )
			if(alg != NEWTON_RAPHSON2)
				if(alg != NEWTON_RAPHSON3)
				if( x*x + y*y > 4)
					break;
	}	   
	   

	if(alg == JULIAN)
	{
		w = a;
		v = b;	
	}
	
	return colours[c];
}


BYTE* ConvertRGBToBMPBuffer ( BYTE* Buffer, int width, int height, long* newsize )
{
	// first make sure the parameters are valid
	if ( ( NULL == Buffer ) || ( width == 0 ) || ( height == 0 ) )
		return NULL;

	// now we have to find with how many bytes
	// we have to pad for the next DWORD boundary	

	int padding = 0;
	int scanlinebytes = width * 3;
	while ( ( scanlinebytes + padding ) % 4 != 0 )     // DWORD = 4 bytes
		padding++;
	// get the padded scanline width
	int psw = scanlinebytes + padding;
	
	// we can already store the size of the new padded buffer
	*newsize = height * psw;

	// and create new buffer
	BYTE* newbuf = new BYTE[*newsize];
	
	// fill the buffer with zero bytes then we dont have to add
	// extra padding zero bytes later on
	memset ( newbuf, 0, *newsize );

	// now we loop trough all bytes of the original buffer, 
	// swap the R and B bytes and the scanlines
	long bufpos = 0;   
	long newpos = 0;
	for ( int y = 0; y < height; y++ )
		for ( int x = 0; x < 3 * width; x+=3 )
		{
			bufpos = y * 3 * width + x;     // position in original buffer
			newpos = ( height - y - 1 ) * psw + x;           // position in padded buffer

			newbuf[newpos] = Buffer[bufpos+2];       // swap r and b
			newbuf[newpos + 1] = Buffer[bufpos + 1]; // g stays
			newbuf[newpos + 2] = Buffer[bufpos];     // swap b and r
		}

	return newbuf;
}

bool SaveBMP ( BYTE* Buffer, int width, int height, long paddedsize, char* bmpfile )
{
	// declare bmp structures 
	BITMAPFILEHEADER bmfh;
	BITMAPINFOHEADER info;
	
	// andinitialize them to zero
	memset ( &bmfh, 0, sizeof (BITMAPFILEHEADER ) );
	memset ( &info, 0, sizeof (BITMAPINFOHEADER ) );
	
	// fill the fileheader with data
	bmfh.bfType = 0x4d42;       // 0x4d42 = 'BM'
	bmfh.bfReserved1 = 0;
	bmfh.bfReserved2 = 0;
	bmfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + paddedsize;
	bmfh.bfOffBits = 0x36;		// number of bytes to start of bitmap bits
	
	// fill the infoheader

	info.biSize = sizeof(BITMAPINFOHEADER);
	info.biWidth = width;
	info.biHeight = height;
	info.biPlanes = 1;			// we only have one bitplane
	info.biBitCount = 24;		// RGB mode is 24 bits
	info.biCompression = BI_RGB;	
	info.biSizeImage = 0;		// can be 0 for 24 bit images
	info.biXPelsPerMeter = 0x0ec4;     // paint and PSP use this values
	info.biYPelsPerMeter = 0x0ec4;     
	info.biClrUsed = 0;			// we are in RGB mode and have no palette
	info.biClrImportant = 0;    // all colors are important

	// now we open the file to write to
	HANDLE file = CreateFile ( bmpfile , GENERIC_WRITE, FILE_SHARE_READ,
		 NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
	if ( file == NULL )
	{
		CloseHandle ( file );
		return false;
	}
	
	// write file header
	unsigned long bwritten;
	if ( WriteFile ( file, &bmfh, sizeof ( BITMAPFILEHEADER ), &bwritten, NULL ) == false )
	{	
		CloseHandle ( file );
		return false;
	}
	// write infoheader
	if ( WriteFile ( file, &info, sizeof ( BITMAPINFOHEADER ), &bwritten, NULL ) == false )
	{	
		CloseHandle ( file );
		return false;
	}
	// write image data
	if ( WriteFile ( file, Buffer, paddedsize, &bwritten, NULL ) == false )
	{	
		CloseHandle ( file );
		return false;
	}
	
	// and close file
	CloseHandle ( file );

	return true;
}

BYTE* LoadRGBFromScreen(HDC hdc)
{
  	// allocate memory to contain the whole file.
	BYTE* buffer;
	long lSize;

  	buffer = (BYTE*) malloc (20000000);
  	if (buffer == NULL) exit (2);

	long i=0;
	int num;
	int xc,yc;

    	for(yc=1; yc <= sh; yc++)
	for(xc=1; xc <= sw; xc++)
 	{	
		COLORREF col = drawRGB(xc,yc);
		BYTE b = (BYTE) GetRValue(col);
		buffer[i]=b; 	
		i++;
		b = (BYTE) GetGValue(col);
		buffer[i]=b; 	
		i++;
		b = (BYTE) GetBValue(col);
		buffer[i]=b; 	
		i++;
	}
  	return buffer;
}

void saveAsBitmap(HWND hwnd)
{

	HDC hdc = GetDC(hwnd);
	BYTE* bufferin = LoadRGBFromScreen(hdc);
	// voglio dare un nome univoco al file BMP
	// utilizzando il tempo
	long m;
	time_t t;
	time(&t);
	char nameFile[20]; 
	struct tm* timeinfo  = localtime(&t);
	sprintf(nameFile, "%u%u%u-%u%u%u.bmp",timeinfo->tm_mday,timeinfo->tm_mon,timeinfo->tm_year,timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);
	BYTE* b = ConvertRGBToBMPBuffer((BYTE*)bufferin,sw,sh,&m);
	SaveBMP(b,sw,sh,m,nameFile);
	delete [] b;
}

// ---------------------------------------------------------------------
#endif

