/**********************************************************************

  Bmp file reader/writer

  bmp.h

  Authors: Eyüp Ensar Kalaycı & Leyla Albayrak
           Karadeniz Technical University Computer Engineering
           2018

**********************************************************************/
#ifndef BMP_H_INCLUDED
#define BMP_H_INCLUDED

typedef unsigned char  byte;            // 1 byte
typedef unsigned short word;            // 2 byte
typedef unsigned int   dword;           // 4 byte

struct Bmph{
    word  bftype;                       // bitmap indicator must be 'BM'
    dword bfsize;                       // bitmap file size
    word  bfres1;                       // reserved bytes
    word  bfres2;                       // reserved bytes
    dword bfoffset;                     // start byte of colors
};

struct Bmih{
    dword bisize;                       // bitmap info size
    dword width;                        // x pixels of bmp
    dword height;                       // y pixels of bmp
    word  biplane;                      // number of planes set to 1
    word  bppixel;                      // bit per pixel
    dword comtype;                      // compression method.bmp Okuma ve Yazma
    dword rawsize;                      // total bytes of color and padding
    dword xpmeter;                      // x pixel per meter-horizontal resolution
    dword ypmeter;                      // y pixel per meter-vertical   resolution
    dword ncolors;                      // number of colors in the palette
    dword icolors;                      // important colors
};

struct Rgba{                            // pixel data (memory aligned = bgr)
    byte blue;                          // blue channel
    byte green;                         // green channel
    byte red;                           // red channel
    byte alpha;                         // alfa channel (not used)
};

class Bmp{
    public:
        Bmp();
        Bmp(const char* file_name);
        Bmp(const Bmp&);
        Bmp(int,int);
        ~Bmp();

        bool loadImage(const char* file_name);          // load bmp from disk
        bool saveImage(const char* file_name)const;     // save bmp to disk
        void newBmp(int Width,int Height);              // create empty bmp space on memory
        Rgba getRgbaPixel(int x,int y)const;            // return pixel as rgba
        void setPixel(int x,int y,Rgba px);             // set pixel from rgba
        int getWidth()const;                            // return width
        int getHeight()const;                           // return hegiht

        void convertToIntensity();                      // convert image to grayscale
        void fillImage(Rgba px);                        // fill whole image with rgba
        Rgba* getRgbaPixels();                          // return pixels adress
        static byte *getValue(Rgba *px, int channel);   // return value of given px by channel

        void operator=(const Bmp&);
        bool operator==(const Bmp&);
        bool operator!=(const Bmp&);

    private:
        bool grayscale;
        Bmph header;
        Bmih bminfo;
        Rgba *pixels;
};

#endif // BMP_H_INCLUDED
