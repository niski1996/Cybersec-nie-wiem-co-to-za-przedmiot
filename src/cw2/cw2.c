#include <stdio.h>
#include <string.h>

static unsigned char byte_key(size_t index, unsigned char seed) {
    return (unsigned char)(seed + (unsigned char)(index * 17u));
}

static void xor_decode(unsigned char *buffer, size_t length, unsigned char seed) {
    for (size_t index = 0; index < length; ++index) {
        buffer[index] = (unsigned char)(buffer[index] ^ byte_key(index, seed));
    }
}

int main(void) {
    const unsigned char seed = 0x4D;
    unsigned char password[] = { 0x3E, 0x3B, 0x04, 0xF2, 0xF4, 0xD6, 0xB3 };
    unsigned char message[] = { 0x18, 0x35, 0x1D, 0xF9, 0xE5, 0xDB, 0x93, 0xB0, 0xB0, 0x8D, 0x84, 0x7C, 0x19 };
    char input[64];

    printf("Podaj haslo: ");
    if (fgets(input, sizeof(input), stdin) == NULL) {
        return 1;
    }

    input[strcspn(input, "\n")] = '\0';

    xor_decode(password, sizeof(password), seed);
    if (strcmp(input, password) == 0) {
        xor_decode(message, sizeof(message), seed);
        puts(message);
    } else {
        puts("Bledne haslo");
    }

    return 0;
}
