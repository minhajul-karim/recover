#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef uint8_t BYTE;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: ./recover source_file\n");
        return 1;
    }

    // Open input file
    FILE *inptr = fopen(argv[1], "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", argv[1]);
        return 2;
    }

    // Declare a buffer of 512 Bytes
    BYTE buffer[512];
    char outfilename[10];
    int jpeg_start = 0, outfile_counter = 0;
    FILE *outptr = NULL;
    
    while(1)
    {
        // Read 512B at a time
        fread(buffer, sizeof(BYTE), 512, inptr);

        // Check for EOF
        if (feof(inptr))
            break;
        else
        {
            // If header matches
            if (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff && (buffer[3] & 0xf0) == 0xe0)
            {
                // If jpeg hasn't started already
                if (jpeg_start == 0)
                {
                    // Create a filename
                    sprintf(outfilename, "%03d.jpg", outfile_counter++);

                    // Open the newly created file to write
                    outptr = fopen(outfilename, "w");
                    if (outptr == NULL)
                    {
                        fprintf(stderr, "Could not open %s.\n", outfilename);
                        return 2;
                    }
                    fwrite(buffer, sizeof(BYTE), 512, outptr);
                    jpeg_start++;
                }

                // If jpeg has started
                else if (jpeg_start == 1)
                {
                    // Close the previous file
                    fclose(outptr);

                    // Create a new filename
                    sprintf(outfilename, "%03d.jpg", outfile_counter++);
                    outptr = fopen(outfilename, "w");
                    if (outptr == NULL)
                    {
                        fprintf(stderr, "Could not open %s.\n", outfilename);
                        return 2;
                    }
                    fwrite(buffer, sizeof(BYTE), 512, outptr);
                }
            }

            // If header doesn't match
            else
            {
                if (jpeg_start == 1)
                    fwrite(buffer, sizeof(BYTE), 512, outptr);
            }
        }
    }

    return 0;
}
