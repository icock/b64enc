/* Returns length of encoded output, null terminator not counted.
 * Returns `-1` when encountering a bug in the code logic.
 */
long b64enc(unsigned char *out, const unsigned char *in, long len);