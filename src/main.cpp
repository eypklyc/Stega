/********************************************************************************

   d888888o. 8888888 8888888888 8 8888888888       ,o888888o.          .8.
 .`8888:' `88.     8 8888       8 8888            8888     `88.       .888.
 8.`8888.   Y8     8 8888       8 8888         ,8 8888       `8.     :88888.
 `8.`8888.         8 8888       8 8888         88 8888              . `88888.
  `8.`8888.        8 8888       8 888888888888 88 8888             .8. `88888.
   `8.`8888.       8 8888       8 8888         88 8888            .8`8. `88888.
    `8.`8888.      8 8888       8 8888         88 8888   8888888 .8' `8. `88888.
8b   `8.`8888.     8 8888       8 8888         `8 8888       .8'.8'   `8. `88888.
`8b.  ;8.`8888     8 8888       8 8888            8888     ,88'.888888888. `88888.
 `Y8888P ,88P'     8 8888       8 888888888888     `8888888P' .8'       `8. `88888.

  An image hiding tool which use exploiting modification direction
  technique described in [1]

  It is the project assignment of the Network and Data Security
  course given by Güzin Ulutaş.

  main.cpp

  Authors: Eyüp Ensar Kalaycı & Leyla Albayrak
           Karadeniz Technical University Computer Engineering
           2018

  [1]   X. Zhang, S.  Wang, Efficient Steganographic Embedding by
        Exploiting  Modification  Direction,  IEEE Communications
        Letters,  Vol.  10,  No.  11,  (2006)  781-783.

**********************************************************************************/
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <limits>
#include <chrono>
#include <vector>
#include <assert.h>
#include "bmp.h"

using namespace std;

// core function of exploiting modification direction,
// evaluate all possibilities to get right digit from modulo
void emd(Rgba *g,int &d,int &m,int &c,int &n)
{
    int sum, f, s;
    do
    {
        sum = 0;
        for(int i = 0; i < n; i++)
            sum += *Bmp::getValue(g+i,c) * (i + 1);
        f = sum % m;
        s = (d - f);

        if(s == 0)
            break;
        else if(s < 0)
            s += m;
        if(s <= n)
        {
            byte *px = Bmp::getValue(g+s-1,c);
            if((*px) != 255)
                (*px)++;
            else
                (*px)--;
        }
        else
        {
            byte *px = Bmp::getValue(g+(m-s)-1,c);
            if((*px) != 0)
                (*px)--;
            else
                (*px)++;
        }
    }
    while(s != 0);
}

// embedding efficiency from [1]
double eEfficiency(int n)
{
    return (2 * n + 1) * log2(2 * n + 1) / (2 * n);
}

// embedding rate from [1]
double eRatio(int n)
{
    return log2(2 * n + 1) / (n);
}

// find error ratio between two images in rgb
double error(Rgba *in, Rgba *out, int size)
{
    double mmae = 0;
    for(int i = 0; i < size; i++)
    {
        mmae += fabs((in[i].red - out[i].red));
        mmae += fabs((in[i].green - out[i].green));
        mmae += fabs((in[i].blue - out[i].blue));
    }
    return mmae / size / 3.0f / 255.0;
}

// calculate mean squared error between two images - mse( r , g , b , (rgb) )
void getMse(Rgba *in, Rgba *out, int size, double *mse)
{
    for(int i = 0; i < size; i++)
    {
        mse[0] += (in[i].red - out[i].red) * (in[i].red - out[i].red);
        mse[1] += (in[i].green - out[i].green) * (in[i].green - out[i].green);
        mse[2] += (in[i].blue - out[i].blue) * (in[i].blue - out[i].blue);
    }
    mse[3] = (mse[0] + mse[1] + mse[2]) / (size * 3.0f);
    mse[0] /= size;
    mse[1] /= size;
    mse[2] /= size;
}

// calculate psnr value given two images - psnr( r , g , b , (rgb) )
void getPsnr(Rgba *in, Rgba *out, int size, double *psnr)
{
    double mse[4] = {0};
    getMse(in, out, size, mse);

    psnr[0] = 10.0f * log10(255 * 255 / mse[0]);
    psnr[1] = 10.0f * log10(255 * 255 / mse[1]);
    psnr[2] = 10.0f * log10(255 * 255 / mse[2]);
    psnr[3] = 10.0f * log10(255 * 255 / mse[3]);
}

// return decimal 255 size in digit given base
int findBlockSize(int base)
{
    int in = 255, block = 0;
    while(in != 0)
    {
        block++;
        in /= base;
    }
    return block;
}

// return optimum (best fit) pixel per digit value from given secret and cover image size (in pixel)
int findOptimumPixelPerDigit(int sSize, int cSize)
{
    // hold perceptual quality and statistical properties, use maximum source space with minimum changes
    // secretSize * block * n + 10 < coverSize
    int n = 1;
    for(n = 1; n < 3125; n++)
    {
        if(sSize * findBlockSize(2 * n + 1) * n + 10 > cSize)
        {
            return n - 1;
            break;
        }
    }
    return n;
}

// check pixel per digit, if it's valid return true.
bool checkPixelPerDigit(int sSize, int cSize, int ppd)
{
    return sSize * findBlockSize(2 * ppd + 1) * ppd + 10 <= cSize;
}

// convert decimal to given base and write it to array.
void dectobase(int in,int *out,int base,int block)
{
    int i = block;
    while(in != 0)
    {
        out[--i] = in % base;
        in /= base;
    }
    while(i > 0)
        out[--i] = 0;
}

// convert array to decimal and return it.
int basetodec(int *in,int base,int block)
{
    int p = 1, out = 0,i = block;
    while(i > 0)
    {
        out += in[--i] * p;
        p *= base;
    }
    return out;
}

// convert rgba pixel channel value to given base and write it to array.
void getPxInBase(Rgba *sPixels,int *inB,int &px, int &c,int &base, int &block)
{
    switch (c) {
    case 0:
        dectobase(sPixels[px].red,inB, base, block);
        break;
    case 1:
        dectobase(sPixels[px].green,inB, base, block);
        break;
    case 2:
        dectobase(sPixels[px].blue,inB, base, block);
        break;
    default:
        dectobase(sPixels[px].red,inB, base, block);
        break;
    }
}

// use Emd embedding algorithm to hide given secret image in cover image.
void embedder(const char *cImagePath, const char *sImagePath,const char *oImagePath, int prefPpd, bool verbose, bool performance)
{
    auto start = chrono::steady_clock::now();

    // read images
    Bmp sImage,cImage;
    if(sImage.loadImage(sImagePath) && cImage.loadImage(cImagePath))
    {
        if(verbose) cout << "  Images successfully readed.\n";
    }
    else
    {
        cout << "  Error in reading images.\n\n";
        return;
    }

    // for performance results copy the cover image before change it.
    Bmp cCopy;
    if(performance) cCopy = cImage;

    // get images pixels adress, calculate images sizes
    Rgba *sPixels = sImage.getRgbaPixels();
    Rgba *cPixels = cImage.getRgbaPixels();
    int secretSize = sImage.getWidth() * sImage.getHeight();
    int coverSize = cImage.getWidth() * cImage.getHeight();

    // check the scret image resolution for hide it in 10 pixel ((44444)5th base = (3124)10th base)
    if(sImage.getHeight() > 3124 || sImage.getWidth() > 3124)
    {
        cout << "\n\n  Maximum secret image resolution is 3124 x 3124.\n\n";
        return;
    }

    // check pixel per digit value
    int pixelPerDigit;
    if(prefPpd == -1)
    {
        pixelPerDigit = findOptimumPixelPerDigit(secretSize, coverSize);
        if(pixelPerDigit == 0)
        {
            int neededCoverSize = secretSize * 6 * 1 + 10;
            cout << "\n\n  Not enough cover space, cover image must contain at least " << neededCoverSize << " pixels."
                 << " (need " << neededCoverSize - coverSize << " more pixels.)\n"
                 << "  Embedding failed.\n\n";
            return;
        }
        if(verbose) cout << "  Optimum pixel per digit: " << pixelPerDigit << endl;
    }
    else
    {
        if(prefPpd < 1 || prefPpd > 3124)
        {
            cout << "\n\n  Pixel per digit value range [1-3124].\n\n";
            return;
        }
        pixelPerDigit = prefPpd;
        if(!checkPixelPerDigit(secretSize, coverSize, pixelPerDigit))
        {
            int neededCoverSize = secretSize * findBlockSize(2 * prefPpd + 1) * prefPpd + 10;
            cout << "\n\n  Not enough cover space, cover image must contain at least " << neededCoverSize << " pixels."
                 << " (need " << neededCoverSize - coverSize << " more pixels.)\n  Try to decrease pixel per digit or use bigger cover image.\n";
            cout << "  Embedding failed.\n\n";
            return;
        }
        if(verbose) cout << "  Prefered pixel per digit: " << pixelPerDigit << endl;
    }

    // embed secret width + height + pixelperdigit to first 10 pixel ( 10px red + 10px green + 10px blue )
    {
        int infoN = 2;
        int infoMod = (2 * infoN + 1);
        int block = 5;
        int info[3][block] = {0};
        dectobase(sImage.getWidth(), info[0], infoMod, block);
        dectobase(sImage.getHeight(), info[1], infoMod, block);
        dectobase(pixelPerDigit, info[2], infoMod, block);

        for(int c = 0; c < 3; c++)
        {
            int cIndex = 0;
            Rgba *g = &cPixels[cIndex];

            int it = 0;
            while(it < block)
            {
                emd(g,info[c][it++],infoMod,c,infoN);
                cIndex += infoN;
                g = &cPixels[cIndex];
            }
        }

    }

    // run emd algorithm with given or calculated pixelperdigit value ( n = pixelperdigit )
    {
        int n = pixelPerDigit;
        int modulo = (2 * n + 1);
        int block = findBlockSize(modulo);
        int inB[block] = {0};

        if(verbose) cout << "  Modulo: " << modulo << endl
                         << "  Block size: " << block << endl
                         << "  Secret image resolution: " << sImage.getWidth() << " x " << sImage.getHeight() << endl
                         << "  Cover image resolution: " << cImage.getWidth() << " x " << cImage.getHeight() << endl;

        assert(secretSize * block * n + 10 <= coverSize);

        cout << "\n  Embedding started.";
        for(int c = 0; c < 3; c++)
        {
            int cIndex = 10;
            Rgba *g = &cPixels[cIndex];
            for(int s = 0; s < secretSize; s++)
            {
                getPxInBase(sPixels,inB,s,c,modulo,block);
                int it = 0;
                while(it < block)
                {
                    emd(g,inB[it++],modulo,c,n);
                    cIndex += n;
                    g = &cPixels[cIndex];
                }
            }
            cout << ".";
        }
        cout << endl;

        if(verbose) cout << "  Embedding secret image is finished. %"
                         << (secretSize * block * n + 10) / (double)coverSize * 100.0 <<" of the cover image was used.\n";
    }
    cImage.saveImage(oImagePath);

    // print performance
    if(performance)
    {
        cout << "\n\t*Embedding efficiency: " << eEfficiency(pixelPerDigit) << endl;
        cout << "\t*Embedding rate: " << eRatio(pixelPerDigit) << endl;
        double psnr[4] = {0};
        getPsnr(cImage.getRgbaPixels(), cCopy.getRgbaPixels(), coverSize, psnr) ;
        cout << "\t*PSNR: " << psnr[3] << " dB (in RGB)" << endl;
        cout << "\t\t*PSNR: " << psnr[0] << " dB (in red)" << endl;
        cout << "\t\t*PSNR: " << psnr[1] << " dB (in green)" << endl;
        cout << "\t\t*PSNR: " << psnr[2] << " dB (in blue)" << endl << endl;
    }

    auto elapsed = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start);
    cout << "  Embedding finished in: " << elapsed.count() << " milliseconds." << endl;
}

void dembedder(const char *cImagePath, const char *oImagePath, bool verbose)
{
    auto start = chrono::steady_clock::now();
    // read images.
    Bmp cImage;
    if(cImage.loadImage(cImagePath))
    {
        if(verbose) cout << "  Image successfully readed.\n";
    }
    else
    {
        cout << "  Error in reading image.\n\n";
        return;
    }

    // get cover pixels
    Rgba *cPixels = cImage.getRgbaPixels();
    int coverSize = cImage.getWidth() * cImage.getHeight();
    int info[3] = {0};
    // read secret image info from first 10 pixels
    {
        int infoN = 2;
        int infoMod = (2 * infoN + 1);
        int sum = 0, f;
        int block = 5;
        int in5[block] = {0};
        int it = 0;
        for(int c = 0; c < 3; c++)
        {
            for(int cIndex = 0;cIndex < 10 ; cIndex+=2)
            {
                Rgba *g = &cPixels[cIndex];
                sum = 0;
                for(int i = 0; i < infoN; i++)
                    sum += *Bmp::getValue(&g[i],c) * (i + 1);

                f = sum % infoMod;
                in5[it++] = f;

                if(it == block)
                {
                    info[c] = basetodec(in5,infoMod,block);
                    it = 0;
                }
            }
        }
    }

    Bmp secret;
    secret.newBmp(info[0],info[1]);
    Rgba *sPixels = secret.getRgbaPixels();
    // run inverse emd algorithm with readed pixelperdigit value ( n = pixelperdigit )
    {
        int n = info[2];
        int modulo = (2 * n + 1);
        int px;
        int index = 0, sum = 0, f;
        int block = findBlockSize(modulo);
        int inB[block] = {0};
        int it = 0;
        if(verbose) cout << "  Pixel per digit: " << n << endl
                         << "  Modulo: " << modulo << endl
                         << "  Block size: " << block << endl
                         << "  Secret image resolution: " << info[0] << " x " << info[1] << endl
                         << "  Cover image resolution: " << cImage.getWidth() << " x " << cImage.getHeight() << endl;
        cout << "\n  De-embedding started.";
        for(int c = 0; c < 3; c++)
        {
            index = sum = 0;
            px = info[0] * info[1];
            for(int cIndex = 10; cIndex < coverSize && px > 0 ; cIndex+=n)
            {
                Rgba *g = &cPixels[cIndex];
                sum = 0;
                for(int i = 0; i < n; i++)
                    sum += *Bmp::getValue(&g[i],c) * (i + 1);

                f = sum % modulo;
                inB[it++] = f;

                if(it == block)
                {
                    *Bmp::getValue(&sPixels[index++],c) = basetodec(inB,modulo,block);
                    it = 0;
                    px--;
                }
            }
            cout << ".";
        }
        cout << endl;
        if(verbose) cout << "  De-embedding secret image is finished.\n";
    }

    secret.saveImage(oImagePath);

    auto elapsed = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start);
    cout << "  De-embedding finished in " << elapsed.count() << " milliseconds." << endl;
}

void test(const char *cImagePath, const char *sImagePath, bool verbose)
{
    Bmp sImage,cImage;
    if(sImage.loadImage(sImagePath) && cImage.loadImage(cImagePath))
    {
        if(verbose) cout << "  Images successfully readed.\n";
    }
    else
    {
        cout << "  Error in reading images.\n\n";
        return;
    }
    if(sImage.getWidth() == cImage.getWidth() && sImage.getHeight() == cImage.getHeight())
        cout << "  The images are %" << 100.0 * (1.0 - error(sImage.getRgbaPixels(),cImage.getRgbaPixels(),sImage.getWidth()*sImage.getHeight())) << " same.\n";
    else
        cout << "  The images are different.\n";
}

void printUsage()
{
    cout << "\n Usage \n\n"
            " Embedding: stega -e [-v -p --pixel-per-digit n] -c cover.bmp -s secret.bmp -o out.bmp\n\n"
            "\t -e, --embed\t\tEmbeds the secret image to cover image.\n"
            "\t -v, --verbose\t\tEnable verbose logging.\n"
            "\t -p, --performance\tShow performance values.\n"
            "\t --pixel-per-digit n\tSets pixel per digit to n.\n"
            "\t -c, --cover-image\tCover image URL.\n"
            "\t -s, --secret-image\tSecret image URL.\n"
            "\t -o, --output-image\tOutput image URL.\n\n"
            " De-embedding: stega -d [-v] -c cover.bmp -o output.bmp\n\n"
            "\t -d, --dembed\t\tDe-embeds the secret image from cover image.\n"
            "\t -v, --verbose\t\tEnable verbose logging.\n"
            "\t -c, --cover-image\tCover image URL.\n"
            "\t -o, --output-image\tOutput image URL.\n\n"
            " Testing: stega -t [-v] image1.bmp image2.bmp\n\n"
            "\t -t, --test\t\tTests how much the given images are equal.\n"
            "\t -v, --verbose\t\tEnable verbose logging.\n\n"
            " Info: stega -i\n\n"
            "\t -i, --info\t\tShows info.\n\n";
}

void printInfo()
{
    cout << "\n\n\n\n\n\t   d888888o. 8888888 8888888888 8 8888888888       ,o888888o.          .8.\n"
            "\t .`8888:' `88.     8 8888       8 8888            8888     `88.       .888.\n"
            "\t 8.`8888.   Y8     8 8888       8 8888         ,8 8888       `8.     :88888.\n"
            "\t `8.`8888.         8 8888       8 8888         88 8888              . `88888.\n"
            "\t  `8.`8888.        8 8888       8 888888888888 88 8888             .8. `88888.\n"
            "\t   `8.`8888.       8 8888       8 8888         88 8888            .8`8. `88888.\n"
            "\t    `8.`8888.      8 8888       8 8888         88 8888   8888888 .8' `8. `88888.\n"
            "\t8b   `8.`8888.     8 8888       8 8888         `8 8888       .8'.8'   `8. `88888.\n"
            "\t`8b.  ;8.`8888     8 8888       8 8888            8888     ,88'.888888888. `88888.\n"
            "\t `Y8888P ,88P'     8 8888       8 888888888888     `8888888P' .8'       `8. `88888.\n\n"
            "\n\t\t  An image hiding tool which use exploiting modification direction\n\t\t  technique described in [1]\n"
            "\n\t\t  Authors: Eyup Ensar Kalayci & Leyla Albayrak 2018\n"
            "\n\t\t  [1]   X. Zhang, S.  Wang, Efficient Steganographic Embedding by\n"
            "\t\t        Exploiting  Modification  Direction,  IEEE Communications\n"
            "\t\t        Letters,  Vol.  10,  No.  11,  (2006)  781-783.\n\n\n\n\n";
}

int main(int argc, char *argv[])
{
    bool verbose        = false;
    bool performance    = false;
    bool embedding      = true;
    bool dembedding     = false;
    bool testing        = false;
    int prefPpd         = -1;
    string cImagePath   = "";
    string sImagePath   = "";
    string oImagePath   = "";

    if (argc < 4) { if(argc == 2)if(strcmp(argv[1], "-i") == 0 || strcmp(argv[1], "--info") == 0){printInfo(); return 0;} printUsage(); return -1;}
    for (int i=1; i<argc; i++)
    {
        if      (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) verbose = true;
        else if (strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--performance") == 0) performance = true;
        else if (strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--test") == 0) {testing = true; embedding = false; dembedding = false; sImagePath = argv[i+1]; i++; cImagePath = argv[i+1]; i++;}
        else if (strcmp(argv[i], "-e") == 0 || strcmp(argv[i], "--embed") == 0) {testing = false; embedding = true; dembedding = false;}
        else if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--dembed") == 0) {testing = false; embedding = false; dembedding = true;}
        else if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--secret-image") == 0) {sImagePath = argv[i+1]; i++;}
        else if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--cover-image") == 0) {cImagePath = argv[i+1]; i++;}
        else if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output-image") == 0) {oImagePath = argv[i+1]; i++;}
        else if (strcmp(argv[i], "--pixel-per-digit") == 0) {prefPpd = stoi(argv[i+1]); i++;}
        else {printUsage(); return -1;}
    }

    if(embedding && cImagePath != "" && sImagePath != "" && oImagePath != "")
        embedder(cImagePath.c_str(),sImagePath.c_str(),oImagePath.c_str(),prefPpd,verbose,performance);
    else if(dembedding && cImagePath != "" && oImagePath != "")
        dembedder(cImagePath.c_str(),oImagePath.c_str(),verbose);
    else if(testing && cImagePath != "" && sImagePath != "")
        test(cImagePath.c_str(),sImagePath.c_str(),verbose);

    return 0;
}
