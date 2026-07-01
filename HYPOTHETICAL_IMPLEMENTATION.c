#include <stddef.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "cccdev_settings.h"




#define MAX_BUFFER_LEN 512


int main() {
    int error;

    int fd = open("/dev/cccdev", O_RDWR);
    // if (fd == 0) { error }

    struct settings settings = {
        .function   = FUNC_ENCRYPT,
        .algorithm  = ALG_CAESAR_TEST,
        .settings   = { .caesar = {.rot = 13 } }
    };
    ioctl(fd, CCCDEV_IOCTL_SET_SETTINGS, &settings);



    char plaintext_buf[] = "My character buffer";
    size_t bytes_written = write(fd, plaintext_buf, sizeof(plaintext_buf));
    // check bytes_written
    printf("PLAINTEXT: [%s]\n", plaintext_buf);


    char ciphertext_buf[MAX_BUFFER_LEN];
    memset(ciphertext_buf, 0, sizeof(ciphertext_buf));
    size_t bytes_read = read(fd, ciphertext_buf, bytes_written);
    // check bytes read
    printf("CIPHER: [%s]\n", ciphertext_buf);


    close(fd);
}
