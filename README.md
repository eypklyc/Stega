# Steganography with Exploiting Modification Direction 

Under this project an image hiding tool was developed with using exploiting modification direction technique as described by researchers Zhang and Wang [1]

The project assignment was carried out in the scope of the course Network and Data Security given by Güzin Ulutaş.

Authors: Eyüp E. Kalaycı, Leyla Albayrak

Computer Engineering, Karadeniz Technical University

* Decision mechanism for best embedding efficiency
* 3124 x 3124 max secret image size
* Option for pixel-per-digit setting
* Test function for comparison
* PSNR calculation
* Works with bmp image
* Optimization for speedup
* Pure C++ no dependency


## Compilation
```
git clone https://github.com/eypklyc/Stega.git stega
cd stega/src
g++ -std=c++11 -o stega main.cpp bmp.cpp
```

## Usage
```
Usage

 Embedding: stega -e [-v -p --pixel-per-digit n] -c cover.bmp -s secret.bmp -o out.bmp

         -e, --embed            Embeds the secret image to cover image.
         -v, --verbose          Enable verbose logging.
         -p, --performance      Show performance values.
         --pixel-per-digit n    Sets pixel per digit to n.
         -c, --cover-image      Cover image URL.
         -s, --secret-image     Secret image URL.
         -o, --output-image     Output image URL.

 De-embedding: stega -d [-v] -c cover.bmp -o output.bmp

         -d, --dembed           De-embeds the secret image from cover image.
         -v, --verbose          Enable verbose logging.
         -c, --cover-image      Cover image URL.
         -o, --output-image     Output image URL.

 Testing: stega -t [-v] image1.bmp image2.bmp

         -t, --test             Tests how much the given images are equal.
         -v, --verbose          Enable verbose logging.

 Info: stega -i

         -i, --info             Shows info.
```

## Example
```
C:\stega>stega -e -o o.bmp -c grass.bmp -s cat.bmp

  Embedding started....
  Embedding finished in: 24 milliseconds.

C:\stega>stega -d -o o2.bmp -c o.bmp

  De-embedding started....
  De-embedding finished in 15 milliseconds.

C:\stega>stega -t o2.bmp cat.bmp
  The images are %100 same.

C:\stega>stega -e -o o.bmp -c grassb.bmp -s tiger.bmp -v -p
  Images successfully readed.
  Optimum pixel per digit: 877
  Modulo: 1755
  Block size: 1
  Secret image resolution: 275 x 183
  Cover image resolution: 7680 x 5748

  Embedding started....
  Embedding secret image is finished. %99.9782 of the cover image was used.

        *Embedding efficiency: 10.7834
        *Embedding rate: 0.0122888
        *PSNR: 77.5624 dB (in RGB)
                *PSNR: 77.5622 dB (in red)
                *PSNR: 77.5652 dB (in green)
                *PSNR: 77.5598 dB (in blue)

  Embedding finished in: 2434 milliseconds.

C:\stega>stega -d -o o2.bmp -c o.bmp -v
  Image successfully readed.
  Pixel per digit: 877
  Modulo: 1755
  Block size: 1
  Secret image resolution: 275 x 183
  Cover image resolution: 7680 x 5748

  De-embedding started....
  De-embedding secret image is finished.
  De-embedding finished in 890 milliseconds.

C:\stega>stega -t o2.bmp tiger.bmp
  The images are %100 same.
```

## References
  [1]   X. Zhang, S.  Wang, Efficient Steganographic Embedding by
        Exploiting  Modification  Direction,  IEEE Communications
        Letters,  Vol.  10,  No.  11,  (2006)  781-783.
        

