#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define KEY "MySecretKey123"   

void rc4_ksa(unsigned char *key, int key_len, unsigned char *S) {
    for (int i = 0; i < 256; i++) S[i] = i;

    int j = 0;
    for (int i = 0; i < 256; i++) {
        j = (j + S[i] + key[i % key_len]) % 256;
        unsigned char temp = S[i];
        S[i] = S[j];
        S[j] = temp;
    }
}

void rc4_crypt(unsigned char *S, unsigned char *data, long data_len) {
    int i = 0, j = 0;
    for (long n = 0; n < data_len; n++) {
        i = (i + 1) % 256;
        j = (j + S[i]) % 256;

        unsigned char temp = S[i];
        S[i] = S[j];
        S[j] = temp;

        unsigned char k = S[(S[i] + S[j]) % 256];
        data[n] ^= k;
    }
}

int main(void) {
    const char *filename = "file.txt";
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        printf("Error: Could not open '%s'.\n", filename);
        return 1;
    }

    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    rewind(fp);

    if (file_size <= 0) {
        printf("Error: File is empty or size could not be determined.\n");
        fclose(fp);
        return 1;
    }

    unsigned char *buffer = (unsigned char *)malloc(file_size);
    if (buffer == NULL) {
        printf("Error: Memory allocation failed.\n");
        fclose(fp);
        return 1;
    }

    size_t bytes_read = fread(buffer, 1, file_size, fp);
    fclose(fp);
    if ((long)bytes_read != file_size) {
        printf("Error: Could not read entire file.\n");
        free(buffer);
        return 1;
    }

    unsigned char S[256];
    unsigned char key[] = KEY;
    rc4_ksa(key, (int)strlen((char *)key), S);
    rc4_crypt(S, buffer, file_size);   

    FILE *fp_out = fopen(filename, "wb");
    if (fp_out == NULL) {
        printf("Error: Could not open '%s' for writing.\n", filename);
        free(buffer);
        return 1;
    }

    size_t bytes_written = fwrite(buffer, 1, file_size, fp_out);
    if ((long)bytes_written != file_size) {
        printf("Error: Could not write entire file.\n");
        fclose(fp_out);
        free(buffer);
        return 1;
    }

    fclose(fp_out);

    free(buffer);

    printf("Decryption complete. %ld bytes written to %s\n", file_size, filename);
    return 0;
}