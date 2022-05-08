#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXBUFLEN 1000

int read_file()
{
    FILE *ptr;
    char ch;
    int s=0;
    char buf[10];
    // Opening file in reading mode
    ptr = fopen("test.txt", "r");

    if (NULL == ptr)
    {
        printf("file can't be opened \n");
    }

    printf("content of this file are \n");

    if (NULL != ptr)
    {
        fseek(ptr, 0, SEEK_END);
        s = ftell(ptr);

        if (0 == s)
        {
            printf("file is empty\n");
            return 0;
        }
        else
        {
            fseek(ptr, 0, SEEK_SET);

            do
            {
                ch = fgetc(ptr);
                printf("%c", ch);

                // Checking if character is not EOF.
                // If it is EOF stop eading.
            } while (ch != '\n');

            fclose(ptr);
            return 1;
        }
    }
    return 0;
}

void send_file_content()
{

    char source[MAXBUFLEN + 1];
    FILE *fp = fopen("test.txt", "r");
    if (fp != NULL)
    {
        size_t newLen = fread(source, sizeof(char), MAXBUFLEN, fp);
        if (ferror(fp) != 0)
        {
            fputs("Error reading file", stderr);
        }
        else
        {
            source[newLen++] = '\0'; /* Just to be safe. */
        }
        printf("%s", source);
        fclose(fp);
    }
}
// Driver code
int main()
{
    int x=0;
    x = read_file();
    printf("%d", x);

    send_file_content();
    return 0;
}
