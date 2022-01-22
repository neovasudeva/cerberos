#include "utils.h"

static inline char* itoa_unsigned(uint64_t value, char* str, uint64_t base) {
    int i = 0;                      

    while (value != 0) {
        int64_t digit = value % base;
        value /= base;              
                                    
        if (digit <= 9) {           
            str[i] = '0' + ((char) digit);                                                            
        } else {                    
            str[i] = 'a' + ((char) digit - 10);
        }
                                    
        i++;
    }

    str[i] = '\0';

    for (int j = 0; j < i / 2; j++) {
        char temp = str[j];
        str[j] = str[i - j - 1];
        str[i - j - 1] = temp;
    }

    return str;
}

/* base 10 by default */
static inline char* itoa_signed(int64_t value, char* str) {
    int i = 0;                      
    int64_t base = 10;

    bool neg = value < 0;
    if (neg) {
        value = -1 * value;
    }

    while (value != 0) {
        int64_t digit = value % base;
        value /= base;              
                                    
        if (digit <= 9) {           
            str[i] = '0' + ((char) digit);                                                            
        } else {                    
            str[i] = 'a' + ((char) digit - 10);
        }
                                    
        i++;
    }

    if (neg) {
        str[i] = '-';
        i++;
    }

    str[i] = '\0';

    for (int j = 0; j < i / 2; j++) {
        char temp = str[j];
        str[j] = str[i - j - 1];
        str[i - j - 1] = temp;
    }

    return str;
}

/*
 * itoa
 *
 * converts integer value into string representation with specified base
 * only handles negative numbers if base = 10                                                         
 * 
 * @param value: value to convert   
 * @param str: buffer to store string in and return                                                   
 * @param base: base system to convert to (between 2 and 36)                                          
 * @param sign: the sign of value 
 */                                 
char* itoa(int64_t value, char* str, int64_t base, sign_t sign) {                                                          
    if (base < 2 || base > 36 || (base != 10 && value < 0)) {                                         
        return str;                 
    }                               
                                
    if (sign == SIGNED) {
        return itoa_signed(value, str);
    } else {
        return itoa_unsigned((uint64_t) value, str, base);
    }
}