#include <stdint.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: ./recover card.raw\n");
        return 1;
    }

    FILE *card = fopen(argv[1], "rb");
    if (card == NULL)
    {
        perror(argv[1]);
        return 1;
    }

    uint8_t block[512];
    FILE *photo = NULL;
    unsigned int number = 0;
    int status = 0;

    while (fread(block, sizeof block, 1, card) == 1)
    {
        if (block[0] == 0xff && block[1] == 0xd8 && block[2] == 0xff &&
            block[3] >= 0xe0 && block[3] <= 0xef)
        {
            if (photo != NULL)
            {
                int close_result = fclose(photo);
                photo = NULL;
                if (close_result != 0)
                {
                    status = 1;
                    break;
                }
            }

            char filename[16];
            snprintf(filename, sizeof filename, "%03u.jpg", number);
            photo = fopen(filename, "wb");
            if (photo == NULL)
            {
                perror(filename);
                status = 1;
                break;
            }
            number++;
        }

        if (photo != NULL && fwrite(block, sizeof block, 1, photo) != 1)
        {
            status = 1;
            break;
        }
    }

    if (ferror(card))
    {
        status = 1;
    }
    if (photo != NULL && fclose(photo) != 0)
    {
        status = 1;
    }
    if (fclose(card) != 0)
    {
        status = 1;
    }
    if (status != 0)
    {
        fprintf(stderr, "Could not finish recovering the photos.\n");
    }

    return status;
}