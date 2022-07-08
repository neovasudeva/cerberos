#include <acpi/sdt.h>

bool verify_sdt_checksum(sdt_header_t* sdt_hdr) {
    uint8_t checksum = 0;
    int8_t* ptr = (int8_t*) sdt_hdr;

    for (uint32_t i = 0; i < sdt_hdr->length; i++) {
        checksum += *ptr;
        ptr++;
    }

    return (checksum & 0xff) == 0;
}