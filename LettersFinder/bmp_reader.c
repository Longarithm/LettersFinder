// via http://stackoverflow.com/questions/1968561/getting-the-pixel-value-of-bmp-file
// Under unix, you may produce BMP files as below by 
// # emerge imagemagick
// $ import filename.bmp3 
//  #
//You need to study the BMP file format. It is easier to read uncompressed 24-bit BMP files. They just contain a header at the beginning and RGB values of each pixel.

//To start with this, check the example of 2x2 bitmap image at http://en.wikipedia.org/wiki/BMP%5Ffile%5Fformat. Follow the below steps.

//   1. Create the 2x2 BMP image shown on Wikipedia.
//   2. Open the file in binary mode using your C program.
//   3. Seek to byte position 54.
//   4. Read 3 bytes.

//The bytes would be 0, 0 and 255 respectively. (Not sure whether the order is RGB. I had done this long back and I think the order is not RGB. Just verify this.)

//As simple as that! Study the header of the BMP to understand more about the format.


//Note: you may need to grab an extra byte for the alpha values if your BMP has alpha channel. In that case image would be image[pixelcount][4], and you would add another getc(streamIn) line to hold that fourth index. My BMP turned out to not need that.

 // super-simplified BMP read algorithm to pull out RGB data
 // read image for coloring scheme
 int image[1024][3]; // first number here is 1024 pixels in my image, 3 is for RGB values
 FILE *streamIn;
 streamIn = fopen("./mybitmap.bmp", "r");
 if (streamIn == (FILE *)0){
   printf("File opening error ocurred. Exiting program.\n");
   exit(0);
 }

 int byte;
 int count = 0;
 for(i=0;i<54;i++) byte = getc(streamIn);  // strip out BMP header

 for(i=0;i<1024;i++){    // foreach pixel
    image[i][2] = getc(streamIn);  // use BMP 24bit with no alpha channel
    image[i][1] = getc(streamIn);  // BMP uses BGR but we want RGB, grab byte-by-byte
    image[i][0] = getc(streamIn);  // reverse-order array indexing fixes RGB issue...
    printf("pixel %d : [%d,%d,%d]\n",i+1,image[i][0],image[i][1],image[i][2]);
 }

 fclose(streamIn);
