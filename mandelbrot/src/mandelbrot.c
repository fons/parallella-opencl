// The modifications porting this code to OpenCL are
// Copyright (c) 2012 Brown Deer Technology, LLC.
//
// Mandelbrot.c
// Written by User:Evercat
//
// This draws the Mandelbrot set and spits out a .bmp file.
// Should be quite portable (endian issues have been taken
// care of, for example)
//
// Released under the GNU Free Documentation License
// or the GNU Public License, whichever you prefer:
// 9 February, 2004.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdcl.h>
#include <errno.h>

#define OUTFILE "./mandelbrot.bmp"

#define WIDTH 1024
#define HEIGHT 768

#define CENTRE_X -0.5
#define CENTRE_Y 0
#define ZOOM 300

#define ITERATIONS 1024  // Higher is more detailed, but slower...

// Plotting functions and parameters...

#define bailoutr(n) (5*n  )
#define bailoutg(n) (20*n  )
#define bailoutb(n) 0

#define min(a,b) ((a<b)?a:b)

// Colours for the set itself...

#define IN_SET_R 0
#define IN_SET_G 0
#define IN_SET_B 0

void drawbmp(int width, int height, unsigned char* pixels, char * filename);

/////////////////////////////////// MAIN PROGRAM ///////////////////////////////////

int main (void)
{

   float startx; float endx;
   float starty; float endy;
   float dx; float dy;
   float dx_over_width,dy_over_height;

   char kern[] = "mandel_kern.cl";
   void * openHandle;

   int iterations = ITERATIONS;
   int width = WIDTH;
   int height = HEIGHT;

   char strInfo[20];
   FILE * pFile;

   pFile = fopen(kern, "r");
   if (pFile == NULL)
   {
    printf("Opening the Kernel file: %s produced an error(%d). Make sure that the source code variable kern has a valid path to the cl code and that the code is readable.\n", kern, errno);
    exit(0);
   }
   else
    fclose(pFile); // only open the file to check that it is there and readable

   pFile = fopen("./debug", "w");

   fprintf(pFile, "About to malloc pixels\n");
   cl_uchar* pixels = (cl_uchar*) clmalloc(stdacc, width * height * 3, 0);

   startx = CENTRE_X - ((float) WIDTH / (ZOOM * 2));
   endx = CENTRE_X + ((float) WIDTH / (ZOOM * 2));

   starty = CENTRE_Y - ((float) HEIGHT / (ZOOM * 2));
   endy = CENTRE_Y + ((float) HEIGHT / (ZOOM * 2));

   fprintf(pFile, "Plotting from (%f, %f) to (%f, %f)\n", startx, starty, endx, endy);

   dx = endx - startx;
   dy = endy - starty;
   dx_over_width = dx / width;
   dy_over_height = dy / height;


   fprintf(pFile, "Opening kernel file:%s\n", kern);
   openHandle = clopen(stdacc, kern, CLLD_NOW);

   fprintf(pFile, "Getting the kernel with clsym\n");
   cl_kernel krn = clsym(stdacc, openHandle, "mandel_kern", CLLD_NOW);

   clGetKernelInfo(krn, CL_KERNEL_FUNCTION_NAME, sizeof(strInfo), strInfo, NULL);
   fprintf(pFile, "The kernel is called: %s\n", strInfo);

   fprintf(pFile, "Calling clndrange\n");
   clndrange_t ndr = clndrange_init1d(0, height, 16);
   fprintf(pFile, "Calling clforka\n");
   clforka(stdacc, 0, krn, &ndr, CL_EVENT_WAIT,
      iterations, width, startx, starty, dx_over_width, dy_over_height, pixels);

   fprintf(pFile, "Transferring memory contents from the Epiphany using clmsync\n");
   clmsync(stdacc, 0, pixels, CL_MEM_HOST|CL_EVENT_WAIT);

   fprintf(pFile, "Calling drawbmp\n");
   drawbmp(width, height, pixels, OUTFILE);

   fprintf(pFile, "Saved bitmap to %s. Done.\n", OUTFILE);
   clfree(pixels);
   fclose(pFile);

   return 0;
}


void drawbmp (int width, int height, unsigned char* pixels, char * filename) {

   unsigned int headers[13];
   FILE * outfile;
   int extrabytes;
   int paddedsize;
   int x; int y; int n;

   extrabytes = 4 - ((width * 3) % 4); // How many bytes of padding to add to
                                       // each horizontal line - the size of
                                       // which must be a multiple of 4 bytes.
   if (extrabytes == 4)
      extrabytes = 0;

   paddedsize = ((width * 3) + extrabytes) * height;

   // Headers...

   headers[0]  = paddedsize + 54;      // bfSize (whole file size)
   headers[1]  = 0;                    // bfReserved (both)
   headers[2]  = 54;                   // bfOffbits
   headers[3]  = 40;                   // biSize
   headers[4]  = width;  // biWidth
   headers[5]  = height; // biHeight
                                       // 6 will be written directly...
   headers[7]  = 0;                    // biCompression
   headers[8]  = paddedsize;           // biSizeImage
   headers[9]  = 0;                    // biXPelsPerMeter
   headers[10] = 0;                    // biYPelsPerMeter
   headers[11] = 0;                    // biClrUsed
   headers[12] = 0;                    // biClrImportant

   outfile = fopen (filename, "wb");

   // Headers begin...
   // When printing ints and shorts, write out 1 character at time to
   // avoid endian issues.

   fprintf (outfile, "BM");

   for (n = 0; n <= 5; n++)
   {
      fprintf(outfile, "%c", headers[n] & 0x000000FF);
      fprintf(outfile, "%c", (headers[n] & 0x0000FF00) >> 8);
      fprintf(outfile, "%c", (headers[n] & 0x00FF0000) >> 16);
      fprintf(outfile, "%c", (headers[n] & (unsigned int) 0xFF000000) >> 24);
   }

   // These next 4 characters are for the biPlanes and biBitCount fields.

   fprintf(outfile, "%c", 1);
   fprintf(outfile, "%c", 0);
   fprintf(outfile, "%c", 24);
   fprintf(outfile, "%c", 0);

   for (n = 7; n <= 12; n++)
   {
      fprintf(outfile, "%c", headers[n] & 0x000000FF);
      fprintf(outfile, "%c", (headers[n] & 0x0000FF00) >> 8);
      fprintf(outfile, "%c", (headers[n] & 0x00FF0000) >> 16);
      fprintf(outfile, "%c", (headers[n] & (unsigned int) 0xFF000000) >> 24);
   }

   // Headers done, now write the data...

   for (y = height - 1; y >= 0; y--)  // BMPs are written bottom to top.
   {
      for (x = 0; x <= width - 1; x++)
      {
         // Also, it's written in (b,g,r) format...

         fprintf(outfile, "%c", pixels[(x * 3) + 2 + (y * width * 3)]);
         fprintf(outfile, "%c", pixels[(x * 3) + 1 + (y * width * 3)]);
         fprintf(outfile, "%c", pixels[(x * 3) + 0 + (y * width * 3)]);
      }
      if (extrabytes) // See above - BMP lines must be of lengths divisible by 4
      {
         for (n = 1; n <= extrabytes; n++)
         {
            fprintf(outfile, "%c", 0);
         }
      }
   }

   fclose (outfile);
   return;
}
