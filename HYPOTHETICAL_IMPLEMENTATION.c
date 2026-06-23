#include <stddef.h>
#include <stdio.h>

#include "settings.h"




#define MAX_BUFFER_LEN 512


int main() {
    int error;

    int fd = open("/dev/cccdev", O_RDWR);
    // if (fd == 0) { error }

    error = ioctl(fd, CONFIG_CRYPT_FUNCTION, FUN_ENCRYPT);
    // check error
    error = ioctl(fd, CONFIG_CRYPT_ALGORITHM, ALG_CAESAR_TEST);
    // check error
    struct caesar_test_settings settings;
    settings.rot = 13;
    error = ioctl(fd, CONFIG_ALGORITHM_SETTINGS, &settings);
    // check error


    char plaintext_buf[] = "My character buffer";
    size_t bytes_written = write(fd, plaintext_buf, sizeof(plaintext_buf));
    // check bytes_written


    char ciphertext_buf[MAX_BUFFER_LEN];
    memset(ciphertext_buf, 0, sizeof(ciphertext_buf));
    size_t bytes_read = read(fd, ciphertext_buf, bytes_written);
    // check bytes read


    close(fd);
}
