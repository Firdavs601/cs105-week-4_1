#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        fprintf(stderr, "Usage: ./volume input.wav output.wav factor\n");
        return 1;
    }

    char *end;
    double factor = strtod(argv[3], &end);
    if (end == argv[3] || *end != '\0' || !isfinite(factor) || factor < 0)
    {
        fprintf(stderr, "Enter a nonnegative volume factor.\n");
        return 1;
    }

    if (strcmp(argv[1], argv[2]) == 0)
    {
        fprintf(stderr, "Choose a different output file.\n");
        return 1;
    }

    FILE *input = fopen(argv[1], "rb");
    if (input == NULL)
    {
        perror(argv[1]);
        return 1;
    }

    FILE *output = fopen(argv[2], "wb");
    if (output == NULL)
    {
        perror(argv[2]);
        fclose(input);
        return 1;
    }

    uint8_t header[44];
    int status = 0;

    if (fread(header, 1, sizeof header, input) != sizeof header ||
        fwrite(header, 1, sizeof header, output) != sizeof header)
    {
        status = 1;
    }

    int16_t sample;
    while (status == 0 && fread(&sample, sizeof sample, 1, input) == 1)
    {
        double scaled = sample * factor;
        if (scaled >= 32768 || scaled <= -32769)
        {
            fprintf(stderr, "Scaled sample is out of range.\n");
            status = 1;
            break;
        }

        sample = (int16_t) scaled;
        if (fwrite(&sample, sizeof sample, 1, output) != 1)
        {
            status = 1;
        }
    }

    if (ferror(input))
    {
        status = 1;
    }
    if (fclose(input) != 0)
    {
        status = 1;
    }
    if (fclose(output) != 0)
    {
        status = 1;
    }
    if (status != 0)
    {
        fprintf(stderr, "Could not finish processing the WAV file.\n");
    }

    return status;
}