#define set_red(n) (5*n  )
#define set_green(n) (20*n  )
#define set_blue(n) 0

__kernel void mandel_kern(
   int iterations,
   int width,
   float startx, 
   float starty, 
   float dx, 
   float dy, 
   __global uchar* pixels
)
{
 int threeXwidth = 3 * width;
   unsigned char line[threeXwidth];
   int i, j, n, m, pixelBase;
   float x, y, r, s, nextr, nexts;

   j = get_global_id(0);

   for (i = 0; i < width; i++) 
   {

      x = startx + i*dx;
      y = starty + j*dy;

      r = x; 
      s = y;

      for (n = 0; n < iterations; n++) 
      {

         nextr = ((r * r) - (s * s)) + x;
         nexts = (2 * r * s) + y;
         r = nextr;
         s = nexts;
         
         if ((r * r) + (s * s) > 4) break;

      }

      if (n == iterations) n=0;

      line[(i * 3) + 0 ] = min(255,set_red(n));
      line[(i * 3) + 1 ] = min(255,set_green(n));
      line[(i * 3) + 2 ] = min(255,set_blue(n));   

   }

 pixelBase = j * threeXwidth;
   for (m =0; m < threeXwidth; m++)
    pixels[pixelBase + m] = line[m];

}
