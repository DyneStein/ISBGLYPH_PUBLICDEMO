#ifndef STRING_H
#define STRING_H

// ========== MANUAL STRING FUNCTIONS (NO CSTRING LIBRARY) ==========

// get string length manually
int stringLength(const char* str) {
    int len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

// copy string manually
void stringCopy(char* dest, const char* src) {
    int i = 0;
    while (src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

// copy string with max length limit
void stringCopyN(char* dest, const char* src, int maxLen) {
    int i = 0;
    while (src[i] != '\0' && i < maxLen - 1) {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

// compare two strings manually
int stringCompare(const char* str1, const char* str2) {
    int i = 0;
    while (str1[i] != '\0' && str2[i] != '\0') {
        if (str1[i] != str2[i]) {
            return str1[i] - str2[i];
        }
        i++;
    }
    return str1[i] - str2[i];
}

// concatenate strings manually
void stringConcat(char* dest, const char* src) {
    int destLen = stringLength(dest);
    int i = 0;
    while (src[i] != '\0') {
        dest[destLen + i] = src[i];
        i++;
    }
    dest[destLen + i] = '\0';
}

// trim whitespace from string
void trim(char* str) {
    int start = 0;
    int end = 0;
    int len = 0;
    
    // find length
    while (str[len] != '\0') len++;
    if (len == 0) return;
    
    // find first non-space character
    while (str[start] == ' ' || str[start] == '\t' || str[start] == '\n' || str[start] == '\r') {
        start++;
    }
    
    // find last non-space character
    end = len - 1;
    while (end >= start && (str[end] == ' ' || str[end] == '\t' || str[end] == '\n' || str[end] == '\r')) {
        end--;
    }
    
    // shift content to beginning
    for (int i = 0; i <= end - start; i++) {
        str[i] = str[start + i];
    }
    str[end - start + 1] = '\0';
}

// convert string to uppercase (for case-insensitive comparison)
void toUpperCase(char* str) {
    int i = 0;
    while (str[i] != '\0') {
        if (str[i] >= 'a' && str[i] <= 'z') {
            str[i] = str[i] - 32;  // convert to uppercase
        }
        i++;
    }
}

// convert string to lowercase
void toLowerCase(char* str) {
    int i = 0;
    while (str[i] != '\0') {
        if (str[i] >= 'A' && str[i] <= 'Z') {
            str[i] = str[i] + 32;  // convert to lowercase
        }
        i++;
    }
}

// case-insensitive string compare
int stringCompareIgnoreCase(const char* str1, const char* str2) {
    int i = 0;
    while (str1[i] != '\0' && str2[i] != '\0') {
        char c1 = str1[i];
        char c2 = str2[i];
        
        // convert to uppercase for comparison
        if (c1 >= 'a' && c1 <= 'z') c1 = c1 - 32;
        if (c2 >= 'a' && c2 <= 'z') c2 = c2 - 32;
        
        if (c1 != c2) {
            return c1 - c2;
        }
        i++;
    }
    
    // handle case where one string is longer
    char c1 = str1[i];
    char c2 = str2[i];
    if (c1 >= 'a' && c1 <= 'z') c1 = c1 - 32;
    if (c2 >= 'a' && c2 <= 'z') c2 = c2 - 32;
    
    return c1 - c2;
}

// normalize user input (trim + uppercase for consistent lookup)
void normalizeInput(char* str) {
    trim(str);
    toUpperCase(str);
}

#endif
