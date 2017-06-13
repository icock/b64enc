#include "b64enc.h"

#define paddingChar '='

const unsigned char base64Table[65] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

long b64enc(unsigned char *out, const unsigned char *in, const long len)
{
    unsigned char *p = out;

    long i;
    for (i = 0; i < len - 2; i += 3)
        /* For base64 (`2 ** 6` bits) encoding, every 3 bytes (8 bits) are grouped together
         * since LCM(6, 8) == 3.
         * To avoid exceeding bound, check `< len - 2`. */
    {
        /* C will fill `0` after shifting for unsigned types,
         * thus to get the first 6 bits of the first byte in the group,
         * we shift 2 (`8 - 6`). */
        *p = base64Table[(in[i] >> 2)];
        p++;
        /* `& 00000011` zeros the first 6 bits already processed, and the last 2 bits are left.
         * These 2 bits left will be the first 2 bits of the next 6 bits to process (`<< 4`).
         * For the second (`i + 1`) byte, `>> 4` produces its first 4 bits.
         * Then we join (`|`) the 2 bits and the 4 bits together. */
        *p = base64Table[((in[i] & 0x03) << 4 | in[i + 1] >> 4)];
        p++;
        /* Similar process applied to the last 4 bits of the second byte in the group
         * and the first 4 bits of the third byte. */
        *p = base64Table[((in[i + 1] & 0x0F) << 2 | in[i + 2] >> 6)];
        p++;
        /* The last 6 bits of the third byte and the group. */
        *p = base64Table[in[i + 2] & 0x3F];
        p++;
    }

    /* Since the loop is over, we have `i >= len - 2` and `i - 3 < len - 2`,
     * thus `len - 2 <= i < len + 1`, i.e. `i` is `len - 2`, `len - 1`, or `len`. */
    if (i == len) /* no padding needed */
    {
        /* Closes output string. */
        *p = '\0';
        /* Returns output string length (null terminator not counted). */
        return p - out;
    }
    else if (i == len - 1) /* 1 byte left alone */
    {
        /* The processing is similar to above except that:
         *
         * 1. The last 2 bits are padded with 4-bit zeros (filled by C automatically).
         * 2. The output are padded with `==`.
         * */
        *p = base64Table[in[i] >> 2];
        p++;
        *p = base64Table[(in[i] & 0x03) << 4];
        p++;

        *p = paddingChar;
        p++;
        *p = paddingChar;
        p++;

        *p = '\0';
        return p - out;
    }
    else if (i == len - 1) /* 1 byte left alone */
    {
        *p = base64Table[(in[i] >> 2)];
        p++;
        *p = base64Table[((in[i] & 0x03) << 4 | in[i + 1] >> 4)];
        p++;
        *p = base64Table[(in[i] & 0x03) << 2];
        p++;

        *p = paddingChar;
        p++;

        *p = '\0';
        return p - out;
    }
    else /* bug */
    {
        return -1;
    }
}
