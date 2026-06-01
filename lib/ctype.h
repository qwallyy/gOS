/* gOS Kernel — Character Classification */

#ifndef GOS_CTYPE_H
#define GOS_CTYPE_H

static inline bool isspace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v';
}

static inline bool isdigit(char c) {
    return c >= '0' && c <= '9';
}

static inline bool isalpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static inline bool isalnum(char c) {
    return isalpha(c) || isdigit(c);
}

static inline bool isxdigit(char c) {
    return isdigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

static inline bool isupper(char c) {
    return c >= 'A' && c <= 'Z';
}

static inline bool islower(char c) {
    return c >= 'a' && c <= 'z';
}

static inline char toupper(char c) {
    return islower(c) ? (c - 'a' + 'A') : c;
}

static inline char tolower(char c) {
    return isupper(c) ? (c - 'A' + 'a') : c;
}

static inline bool isprint(char c) {
    return c >= 0x20 && c <= 0x7E;
}

static inline bool isgraph(char c) {
    return c >= 0x21 && c <= 0x7E;
}

static inline bool iscntrl(char c) {
    return (c >= 0x00 && c <= 0x1F) || c == 0x7F;
}

static inline bool ispunct(char c) {
    return isgraph(c) && !isalnum(c);
}

static inline bool isblank(char c) {
    return c == ' ' || c == '\t';
}

#endif /* GOS_CTYPE_H */
