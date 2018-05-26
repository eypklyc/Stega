/**********************************************************************

  Bmp file reader/writer

  bmp.cpp

  Authors: Eyüp Ensar Kalaycı & Leyla Albayrak
           Karadeniz Technical University Computer Engineering
           2018

**********************************************************************/
#include "bmp.h"

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <algorithm>

Bmp::Bmp()
{
    grayscale = false;
    pixels = NULL;

    bminfo.width = 0;
    bminfo.height = 0;
}

Bmp::Bmp(const char *file_name)
{
    pixels = NULL;
    grayscale = false;
    loadImage(file_name);
}

Bmp::Bmp(const Bmp& img)
{
    pixels = NULL;
    *this = img;
}

Bmp::Bmp(int, int)
{
    grayscale = false;
}

Bmp::~Bmp()
{
    delete [] pixels;
}

bool Bmp::loadImage(const char *file_name)
{
    // delete if already have allocation
    if(pixels != NULL)
        delete [] pixels;

    std::ifstream bmp(file_name,std::ios::binary);
    if(!bmp.is_open())
        return false;

    // read header and info
    bmp.read((char*)&header.bftype,2);
    bmp.read((char*)&header.bfsize,4);
    bmp.read((char*)&header.bfres1,2);
    bmp.read((char*)&header.bfres2,2);
    bmp.read((char*)&header.bfoffset,4);
    bmp.read((char*)&bminfo.bisize,4);
    bmp.read((char*)&bminfo.width,4);
    bmp.read((char*)&bminfo.height,4);
    bmp.read((char*)&bminfo.biplane,2);
    bmp.read((char*)&bminfo.bppixel,2);
    bmp.read((char*)&bminfo.comtype,4);
    bmp.read((char*)&bminfo.rawsize,4);
    bmp.read((char*)&bminfo.xpmeter,4);
    bmp.read((char*)&bminfo.ypmeter,4);
    bmp.read((char*)&bminfo.ncolors,4);
    bmp.read((char*)&bminfo.icolors,4);

    // find padding bytes
    byte padding = 4 - (3 * bminfo.width) % 4;
    if(padding == 4)
        padding = 0;

    // cursor sets the start of colors
    bmp.seekg((int)header.bfoffset,std::ios::beg);

    // allocating memory for pixels
    pixels = new Rgba [bminfo.width * bminfo.height];

    // creating buffer for speed
    word buffersize = bminfo.width * 3 + padding;
    byte *buffer;
    buffer = new byte [buffersize];

    for(int y = bminfo.height - 1; y >= 0; y--)
    {
        bmp.read((char*)buffer,buffersize);
        for(int x=0 ; x < bminfo.width ; x++ )
        {
            // copying block of memory
            memcpy((char*)&(pixels[bminfo.width * y + x]), buffer + 3 * x, 3);
        }
    }

    delete [] buffer;
    bmp.close();

    return true;
}

bool Bmp::saveImage(const char *file_name)const
{
    std::ofstream bmp(file_name,std::ios::binary);
    if(!bmp.is_open())
        return false;

    byte padding = 4 - (3 * bminfo.width) % 4;
    if(padding == 4)
        padding = 0;

    // write header and info
    bmp.write((char*)&header.bftype,2);
    bmp.write((char*)&header.bfsize,4);
    bmp.write((char*)&header.bfres1,2);
    bmp.write((char*)&header.bfres2,2);
    bmp.write((char*)&header.bfoffset,4);
    bmp.write((char*)&bminfo.bisize,4);
    bmp.write((char*)&bminfo.width,4);
    bmp.write((char*)&bminfo.height,4);
    bmp.write((char*)&bminfo.biplane,2);
    bmp.write((char*)&bminfo.bppixel,2);
    bmp.write((char*)&bminfo.comtype,4);
    bmp.write((char*)&bminfo.rawsize,4);
    bmp.write((char*)&bminfo.xpmeter,4);
    bmp.write((char*)&bminfo.ypmeter,4);
    bmp.write((char*)&bminfo.ncolors,4);
    bmp.write((char*)&bminfo.icolors,4);

    // calculate padding bytes
    word buffersize = bminfo.width * 3 + padding;
    byte *buffer;
    buffer = new byte [buffersize];

    // for padding bytes
    for(int x = 0; x < buffersize; x++)
        buffer[x] = 0;

    // write pixels to memory
    for(int y = bminfo.height - 1; y >= 0; y--){
        for(int x = 0; x < bminfo.width; x++)
            memcpy((char*)buffer + 3 * x, (char*)&(pixels[bminfo.width * y + x]), 3);
        bmp.write((char*)buffer, buffersize);
    }

    delete [] buffer;
    bmp.close();

    return true;
}

void Bmp::newBmp(int Width,int Height)
{
    // set width and height
    bminfo.width = Width;
    bminfo.height = Height;

    if(pixels != NULL)
        delete [] pixels;

    // allocating memory for RGBA pixels
    pixels = new Rgba [Width * Height];

    // calculating padding bytes
    byte padding = 4 - (3 * Width) % 4;
    if(padding == 4)
        padding = 0;

    // calculating row size for bmp size
    dword rowSize = padding + (bminfo.bppixel * Width) / 8;

    header.bftype = 0x4D42;                                 // bu tag yanlis olursa windows dosyayi tanimiyor !!
    header.bfsize = (dword)54+rowSize*Height;               // bmp file size
    header.bfres1 = 0;                                      // reserve
    header.bfres2 = 0;                                      // reserve
    header.bfoffset = 54;                                   // beginning of pixels bytes
    bminfo.bisize = 40;                                     // bitmapinfoheader size
    bminfo.biplane = 1;                                     // number of planes in the image, must be 1
    bminfo.bppixel = 24;                                    // bit per pixel
    bminfo.comtype = 0;                                     // compression type (0=none, 1=RLE-8, 2=RLE-4)
    bminfo.rawsize = rowSize * Height;                      // size of image data in bytes (including padding)
    bminfo.xpmeter = 0;                                     // horizontal resolution in pixels per meter (unreliable)
    bminfo.ypmeter = 0;                                     // vertical resolution in pixels per meter (unreliable)
    bminfo.ncolors = pow(2, bminfo.bppixel);                // number of colors in image, or zero
    bminfo.icolors = 0;                                     // number of important colors, or zero
}

Rgba Bmp::getRgbaPixel(int x,int y)const
{
    if(x < 0 || x > bminfo.width - 1 || y < 0 || y > bminfo.height - 1)
    {
        std::cout << "getRgbaPixel() out of range! " << x << " " << y << std::endl;
        return Rgba();
    }
    return pixels[bminfo.width * y + x];
}
void Bmp::setPixel(int x,int y,Rgba px)
{
    if(x < 0 || x > bminfo.width - 1 || y < 0 || y > bminfo.height - 1)
    {
        std::cout << "setPixel() out of range! " << x << " " << y << std::endl;
        return;
    }
    pixels[bminfo.width * y + x] = px;
}

int Bmp::getWidth()const
{
    return bminfo.width;
}
int Bmp::getHeight()const
{
    return bminfo.height;
}

void Bmp::operator=(const Bmp& img)
{
    if(this->pixels != NULL)
    {
        delete [] pixels;
        pixels = NULL;
    }

    this->header = img.header;
    this->bminfo = img.bminfo;
    this->grayscale = img.grayscale;

    int width = img.bminfo.width;
    int height = img.bminfo.height;

    pixels = new Rgba[height*width];

    // deep copy
    int size = bminfo.width * bminfo.height;
    while(size--)
        memcpy(pixels + size, img.pixels + size, sizeof(Rgba));
}

bool Bmp::operator==(const Bmp& img)
{
    if(this->bminfo.width != img.bminfo.width || this->bminfo.height != img.bminfo.height)
        return false;
    else
    {
        int size = bminfo.width * bminfo.height;
        while(size--)
            if(pixels[size].blue != img.pixels[size].blue ||
               pixels[size].green != img.pixels[size].green ||
               pixels[size].red != img.pixels[size].red)
                return false;
    }
    return true;
}

bool Bmp::operator!=(const Bmp& img)
{
    return !(*this == img);
}

void Bmp::convertToIntensity()
{
    int intensity;
    int size = bminfo.width * bminfo.height;
    while(size--)
    {
        intensity = pixels[size].blue * 0.3 + pixels[size].green * 0.59 + pixels[size].red * 0.11;
        memset(pixels + size, intensity, sizeof(Rgba));
    }
    grayscale = true;
}

void Bmp::fillImage(Rgba px)
{
    int size = bminfo.width * bminfo.height;
    while(size--)
        memcpy(pixels + size, &px, sizeof(Rgba));
}

Rgba *Bmp::getRgbaPixels()
{
    return pixels;
}

byte *Bmp::getValue(Rgba *px, int channel)
{
    switch (channel) {
    case 0:
        return &px->red;
        break;
    case 1:
        return &px->green;
        break;
    case 2:
        return &px->blue;
        break;
    default:
        return &px->red;
        break;
    }
}
