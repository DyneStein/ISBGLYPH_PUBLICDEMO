#ifndef UTILS_H
#define UTILS_H

#include "String.h"
#include <iostream>
#include <fstream>
using namespace std;

// ========== LOCATION UTILITIES ==========

// Parse sector notation like "G-10/4" into sector and subsector
void parseSectorNotation(const char* input, char* sector, char* subsector) {
    int i = 0;
    int j = 0;
    
    // Copy until '/' or end
    while (input[i] != '\0' && input[i] != '/') {
        sector[j++] = input[i++];
    }
    sector[j] = '\0';
    
    // If there's a subsector
    if (input[i] == '/') {
        i++; // skip '/'
        j = 0;
        while (input[i] != '\0') {
            subsector[j++] = input[i++];
        }
        subsector[j] = '\0';
    } else {
        subsector[0] = '\0';
    }
}

// Generate unique ID (simple counter-based)
int globalIDCounter = 1000;

void generateUniqueID(const char* prefix, char* output) {
    // Create ID like "BUS1001", "STOP1002", etc.
    stringCopy(output, prefix);
    int len = stringLength(output);
    
    int num = globalIDCounter++;
    // Simple int to string conversion
    char numStr[20];
    int idx = 0;
    if (num == 0) {
        numStr[idx++] = '0';
    } else {
        int temp = num;
        int digits = 0;
        while (temp > 0) {
            digits++;
            temp /= 10;
        }
        for (int i = digits - 1; i >= 0; i--) {
            numStr[i] = '0' + (num % 10);
            num /= 10;
        }
        idx = digits;
    }
    numStr[idx] = '\0';
    
    stringConcat(output, numStr);
}

// ========== STRING UTILITIES ==========


// ========== MANUAL MATH FUNCTIONS (NO CMATH LIBRARY) ==========

// pi constant
const double PI = 3.14159265358979323846;

// manual square root using newton's method
double squareRoot(double n) {
    if (n < 0) return 0;
    if (n == 0) return 0;
    
    double x = n;
    double y = 1;
    double precision = 0.000001;
    
    while (x - y > precision) {
        x = (x + y) / 2;
        y = n / x;
    }
    return x;
}

// absolute value
double absoluteValue(double x) {
    if (x < 0) return -x;
    return x;
}

// power function
double power(double base, int exp) {
    double result = 1.0;
    int absExp = exp < 0 ? -exp : exp;
    
    for (int i = 0; i < absExp; i++) {
        result *= base;
    }
    
    if (exp < 0) return 1.0 / result;
    return result;
}

// sine using taylor series (keeping it simple for small angles)
double sine(double x) {
    // normalize angle to -PI to PI
    while (x > PI) x -= 2 * PI;
    while (x < -PI) x += 2 * PI;
    
    // taylor series: sin(x) = x - x^3/3! + x^5/5! - x^7/7! ...
    double term = x;
    double sum = term;
    
    for (int i = 1; i < 10; i++) {
        term *= -x * x / ((2 * i) * (2 * i + 1));
        sum += term;
    }
    return sum;
}

// cosine using taylor series
double cosine(double x) {
    // normalize angle to -PI to PI
    while (x > PI) x -= 2 * PI;
    while (x < -PI) x += 2 * PI;
    
    // taylor series: cos(x) = 1 - x^2/2! + x^4/4! - x^6/6! ...
    double term = 1;
    double sum = term;
    
    for (int i = 1; i < 10; i++) {
        term *= -x * x / ((2 * i - 1) * (2 * i));
        sum += term;
    }
    return sum;
}

// arctangent2 (keeping it simple)
double arctan2(double y, double x) {
    double absY = absoluteValue(y);
    double r;
    
    if (x == 0 && y == 0) return 0;
    
    if (absoluteValue(x) > absY) {
        r = absY / absoluteValue(x);
        // arctan approximation
        double atan = r - (r * r * r) / 3.0 + (r * r * r * r * r) / 5.0;
        if (x < 0) atan = PI - atan;
        if (y < 0) atan = -atan;
        return atan;
    } else {
        r = absoluteValue(x) / absY;
        double atan = PI / 2.0 - (r - (r * r * r) / 3.0 + (r * r * r * r * r) / 5.0);
        if (y < 0) atan = -atan;
        if (x < 0) {
            if (y > 0) atan = PI - atan;
            else atan = -PI - atan;
        }
        return atan;
    }
}

// ========== NUMERIC CONVERSIONS ==========

// convert string to double
double stringToDouble(const char* str) {
    double result = 0.0;
    double fraction = 0.0;
    int sign = 1;
    int i = 0;
    bool afterDecimal = false;
    double divisor = 10.0;
    
    if (str[0] == '-') {
        sign = -1;
        i = 1;
    }
    
    while (str[i] != '\0') {
        if (str[i] == '.') {
            afterDecimal = true;
            i++;
            continue;
        }
        
        if (str[i] >= '0' && str[i] <= '9') {
            if (afterDecimal) {
                fraction += (str[i] - '0') / divisor;
                divisor *= 10.0;
            } else {
                result = result * 10.0 + (str[i] - '0');
            }
        }
        i++;
    }
    
    return sign * (result + fraction);
}

// convert string to int manually
int stringToInt(const char* str) {
    int result = 0;
    int sign = 1;
    int i = 0;
    
    if (str[0] == '-') {
        sign = -1;
        i = 1;
    }
    
    while (str[i] != '\0') {
        if (str[i] >= '0' && str[i] <= '9') {
            result = result * 10 + (str[i] - '0');
        }
        i++;
    }
    
    return sign * result;
}

// ========== CSV PARSING ==========

// parse CSV line into tokens manually
int parseCSVLine(char* line, char tokens[][200], int maxTokens) {
    int tokenCount = 0;
    int tokenPos = 0;
    bool inQuotes = false;
    int i = 0;
    
    while (line[i] != '\0' && tokenCount < maxTokens) {
        if (line[i] == '"') {
            inQuotes = !inQuotes;
            i++;
            continue;
        }
        
        if (line[i] == ',' && !inQuotes) {
            tokens[tokenCount][tokenPos] = '\0';
            trim(tokens[tokenCount]);
            tokenCount++;
            tokenPos = 0;
            i++;
            continue;
        }
        
        tokens[tokenCount][tokenPos++] = line[i];
        i++;
    }
    
    // add last token
    if (tokenPos > 0 || tokenCount < maxTokens) {
        tokens[tokenCount][tokenPos] = '\0';
        trim(tokens[tokenCount]);
        tokenCount++;
    }
    
    return tokenCount;
}

// ========== DISTANCE CALCULATION ==========

// calculate distance between coordinates using haversine formula
// implemented manually without cmath library
double calculateDistance(double lat1, double lon1, double lat2, double lon2) {
    double R = 6371.0; // earth radius in km
    
    // convert to radians manually
    double lat1Rad = lat1 * PI / 180.0;
    double lat2Rad = lat2 * PI / 180.0;
    double deltaLat = (lat2 - lat1) * PI / 180.0;
    double deltaLon = (lon2 - lon1) * PI / 180.0;
    
    // haversine formula using manual trig functions
    double sinDeltaLat = sine(deltaLat / 2.0);
    double sinDeltaLon = sine(deltaLon / 2.0);
    
    double a = sinDeltaLat * sinDeltaLat +
               cosine(lat1Rad) * cosine(lat2Rad) *
               sinDeltaLon * sinDeltaLon;
    
    double sqrtA = squareRoot(a);
    double sqrt1MinusA = squareRoot(1.0 - a);
    double c = 2.0 * arctan2(sqrtA, sqrt1MinusA);
    
    return R * c;
}

// ========== INPUT VALIDATION UTILITIES ==========

// Validate CNIC format: XXXXX-XXXXXXX-X (5 digits, 7 digits, 1 digit)
bool isValidCNIC(const char* cnic) {
    int len = stringLength(cnic);
    
    // Expected format: XXXXX-XXXXXXX-X (length should be 15)
    if (len != 15) {
        return false;
    }
    
    // Check format: 5 digits, dash, 7 digits, dash, 1 digit
    for (int i = 0; i < 15; i++) {
        if (i == 5 || i == 13) {
            // Should be dash
            if (cnic[i] != '-') return false;
        } else {
            // Should be digit
            if (cnic[i] < '0' || cnic[i] > '9') return false;
        }
    }
    
    return true;
}

// Validate age range (0-120)
bool isValidAge(int age) {
    return age >= 0 && age <= 120;
}

// Validate rating (0.0-5.0)
bool isValidRating(double rating) {
    return rating >= 0.0 && rating <= 5.0;
}

// Check if string contains only digits
bool isNumeric(const char* str) {
    if (str[0] == '\0') return false;
    
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] < '0' || str[i] > '9') {
            return false;
        }
    }
    return true;
}

// Check if string is valid (non-empty, reasonable length)
bool isValidString(const char* str, int minLen = 1, int maxLen = 100) {
    int len = stringLength(str);
    
    if (len < minLen || len > maxLen) {
        return false;
    }
    
    // Check not all spaces
    bool hasNonSpace = false;
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] != ' ' && str[i] != '\t') {
            hasNonSpace = true;
            break;
        }
    }
    
    return hasNonSpace;
}

// Validate choice input (clears invalid input)
bool isValidChoice() {
    if (cin.fail()) {
        cin.clear();
        cin.ignore(10000, '\n');
        return false;
    }
    return true;
}

#endif
