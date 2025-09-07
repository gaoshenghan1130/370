// EECS 370 SP 23 - Lab 2
// MODIFY AND SUBMIT THIS FILE
#include "numHighBits.h"

// Takes in an integer as an argument, and returns the number of bits set high in its binary representation
int numHighBits(int input){
    int count = 0;
    while (input) {
        count += input & 1; // Increment count if the least significant bit is 1
        input >>= 1;        // Right shift input to check the next bit
    }
    return count;
}
