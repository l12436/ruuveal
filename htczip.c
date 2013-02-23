#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <bits/byteswap.h>

#include "htczip.h"

static inline unsigned short swap(unsigned short s)
{
    return __bswap_constant_16(s);
}

static void fix_mainver_string(char *mainver, int size)
{
    int i;

    for(i=0; i < size - 1; i++) {
        if(mainver[i] == '\xff') {
            mainver[i] = mainver[i+1];
            mainver[i+1] = '\xff';
        }
    }

    if(mainver[size - 1] == '\xff') {
        mainver[size - 1] = 0;
    }
}

int htc_zip_init_header(htc_zip_header_t *header)
{
    memset(header, 0xff, sizeof(*header));
    memset(header->magic, 0, sizeof(header->magic));
    strcpy(header->magic, HTC_ZIP_HEADER_MAGIC);
    header->keymap_index = HTC_ZIP_HEADER_DEFAULT_KEYMAP;
    header->chunks = HTC_ZIP_HEADER_DEFAULT_CHUNKS;
    
    strncpy(header->mainver, HTC_ZIP_HEADER_DEFAULT_MAINVER, 
            sizeof(header->mainver));
}

int htc_zip_read_header(FILE *in, htc_zip_header_t *header)
{
    if(fread(header, 1, sizeof(*header), in) != sizeof(*header)) {
        perror("failed to read htc zip header");
        return 0;
    }

    if(strncmp(header->magic, HTC_ZIP_HEADER_MAGIC, 
               strlen(HTC_ZIP_HEADER_MAGIC))) {
        return 0;
    }

    fix_mainver_string(header->mainver, HTC_ZIP_HEADER_MAINVER_SIZE);

    header->keymap_index = swap(header->keymap_index);
    return 1;
}

int htc_zip_write_header(FILE *out, htc_zip_header_t *header)
{
    int rc = 1;

    header->keymap_index = swap(header->keymap_index);
    
    if(fwrite(header, 1, sizeof(*header), out) != sizeof(*header)) {
        perror("failed to write htc zip header");
        rc = 0;
    }

    header->keymap_index = swap(header->keymap_index);
    return rc;
}