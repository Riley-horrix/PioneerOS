#include "common/string.h"
#include "common/types.h"

/**
 * @brief Find the first occurrence of the character `c` in the string, or NULL if no such character was found.
 * 
 * @param str The string to search through.
 * @param c The character to find.
 * @return const char* Pointer to the character, or NULL.
 */
const char* strchr(const char* str, char c) {
    char head;
    while ((head = *str++) != '\0' && head != c)
        ;
    return head == '\0' ? (const char*)NULL : --str;
}

/**
 * @brief Compare the first `n` characters of two strings.
 * 
 * @param fst The first string to compare.
 * @param snd The second string to compare.
 * @param n The number of characters to compare.
 * @return 0 If both strings are equal.
 * @return >0 If the first string is greater than the second.
 * @return <0 If the second string is greater than the first.
 */
int strncmp(const char* fst, const char* snd, unsigned int n) {

    char c;
    while(n-- > 0 && (c = *fst++) == *snd++) {
        if (c == '\0') {
            return 0;
        }
    }

    return n == 0 ? 0 : (
        *--fst > *--snd ? 1 : -1
    );
}

char* strncpy(char* destination, const char* source, size_t num) {
    while (num-- > 0) {
        char c = *source++;
        *destination++ = c;
        if (c == '\0') {
            // Pad with 0s until num == 0
            while (num-- > 0) {
                *destination++ = (char)0;
            }
            break;
        }
    }

    return destination;
}