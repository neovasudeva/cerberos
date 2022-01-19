#include "string.h"

/*                                                                                                   
 * memcpy                                                                                            
 * copies num bytes from src to dest                                                                 
 * @param num: number of bytes to copy                                                               
 * @param src: src buffer to copy from                                                               
 * @param dest: destination buffer to copy to                                                        
 */                                                                                                  
void* memcpy(void* dest, const void* src, size_t num) {                                              
    char* d = dest;                                                                                  
    const char* s = src;                                                                             
    for (size_t i = 0; i < num; i++) {                                                               
        d[i] = s[i];                                                                                 
    }                                                                                                

    return dest;
}                                                                                                    
                                                                                                     
/*                                                                                                   
 * memmove                                                                                           
 * moves num bytes from src to dest. Deals with case where dest and src                              
 * addresses partially overlap                                                                       
 * @param num: number of bytes to copy                                                               
 * @param src: src buffer to copy from                                                               
 * @param dest: destination buffer to copy to                                                        
 */                                                                                                  
void* memmove(void* dest, const void* src, size_t num) {                                             
    char* d = dest;                                                                                  
    const char* s = src;                                                                             
    char temp[num];                                                                                  
                                                                                                     
    for (size_t i = 0; i < num; i++) {                                                               
        temp[i] = s[i];                                                                              
    }                                                                                                
                                                                                                     
    for (size_t i = 0; i < num; i++) {                                                               
        d[i] = temp[i];                                                                              
    }

    return dest;                                                                            
}                           

/*                                                                                                   
 * memset
 * sets n bytes of memory in buf to c
 * @param buf: memory buffer                                                               
 * @param c: data to set memory to                                                                
 * @param n: number of bytes to set
 */                                                                                                  
void memset(void* buf, int c, size_t n) {
    char* b = (char*) buf;
    for (size_t i = 0; i < n; i++) {
        b[i] = (char) c;
    }
}


/*                                                                                                   
 * strcpy                                                                                            
 * copies string from src to dest                                                                    
 * @param src: src buffer to copy from                                                               
 * @param dest: destination buffer to copy to                                                        
 */                                                                                                  
char* strcpy(char* dest, const char* src) {                                                          
    int i = 0;                                                                                       
    while (src[i] != '\0') {                                                                         
        dest[i] = src[i];                                                                            
        i++;                                                                                         
    }
    dest[i] = '\0';

    return dest;
}

/*
 * strlen
 * gives length of string src
 * @param src: string to find length of 
 */
size_t strlen(const char* src) {
    size_t len = 0;
    while (src[len] != '\0') {
        len++;
    }
    return len;
}

/*
 * itoa
 * converts integer value into string representation with specified base
 * only handles negative numbers if base = 10                                                         
 * @param value: value to convert                                                                     
 * @param str: buffer to store string in and return                                                   
 * @param base: base system to convert to (between 2 and 36)                                          
 */                                                                                                   
char* itoa(int value, char* str, int base) {                                                          
    if (base < 2 || base > 36 || (base != 10 && value < 0)) {                                         
        return str;                                                                                   
    }                                                                                                 
                                                                                                      
    int i = 0;                                                                                        
    bool neg = value < 0;                                                                             
    if (neg) {                                                                                        
        value = -1 * value;                                                                           
    }                                                                                                 
                                                                                                      
    while (value != 0) {                                                                              
        int digit = value % base;                                                                     
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

    // reverse string 
    // i = length of str
    for (int j = 0; j < i / 2; j++) {
        char temp = str[j];
        str[j] = str[i - j - 1];
        str[i - j - 1] = temp;
    }

    return str;
}
