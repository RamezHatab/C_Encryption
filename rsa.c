#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <stdint.h>
#include <string.h>

typedef struct uint128 {
  uint64_t hi;
  uint64_t lo;
} uint128;

uint128 add(uint128 x, uint128 y){
    uint128 sum;
    sum.lo = x.lo + y.lo;
    sum.hi = x.hi + y.hi + (sum.lo < x.lo);
    return sum;
}

uint128 sub(uint128 base, uint128 mod, int overflow){
    uint128 diff;
    diff.lo = base.lo - mod.lo;
    if(overflow){
        diff.hi = diff.hi ^ (0x01LL << 63);
    }

    diff.hi = base.hi - mod.hi - (base.lo < mod.lo);
    return diff;
}

int compareShifted(uint128 mod, uint128 base) {
    return mod.hi <= (base.lo >> 1) ||
           (mod.hi == (base.hi >> 1) && mod.lo <= ((base.lo >> 1) | (base.hi << 63)));
}

// Helper function to shift left (mod * 2)
void shiftLeft(uint128 *x) {
    x->hi = (x->hi << 1) | (x->lo >> 63);
    x->lo <<= 1;
}

// Helper function to shift right (mod / 2)
void shiftRight(uint128 *x) {
    x->lo = (x->lo >> 1) | (x->hi << 63);
    x->hi >>= 1;
}

uint128 modFun(uint128 base, uint128 mod, int overflow){
    uint128 x = mod;
    //if overflow, fix base
    if(overflow){
        base = sub(base, mod, overflow);
    }
    while(compareShifted(mod, base)){
        if(!(mod.hi == 0 && base.hi == 0)){
            shiftLeft(&x);
        }
    }

    while(base.hi >= mod.hi || (base.hi == mod.hi && base.lo >= mod.lo)){
        if(base.hi >= mod.hi || (base.hi == x.hi && base.lo >= x.lo)){
            base = sub(base, x, 0);
        }

        shiftRight(&x);
    }

    return base;
}

uint128 doubleOverflow(uint128 x, uint128 y){
    int overflow = x.hi >> 63;
    x = add(x, x);
    if(overflow){
        x = sub(x, y, overflow);
    }
    return x;
}

uint128 multiply(uint128 x, uint128 y, uint128 mod){
    uint128 prod = {0, 0};
    //we need to check for overflow during mul
    int overflow = 0;
    x = modFun(x, mod, overflow);

    while(y.hi > 0 || y.lo > 0){ //while base > 0
        if(y.lo & 1){ //if odd
            //we need to see if we are going to have overflow
            if((add(prod, x).hi < prod.hi) || add(prod, x).hi < x.hi){
                //this means that the hi bits went down, rmeainig we had overflow
                overflow = 1;
            }

            uint128 temp = add(prod, x);
            prod = modFun(temp, mod, overflow);
        }

        //this is where we check for a double overflow (ayush)
        uint128 dOF = doubleOverflow(x, mod);
        overflow = 0;
        x = modFun(dOF, mod, overflow);

        shiftRight(&y);
    }

    return prod;
}

uint128 mod_exp(uint128 base, uint128 e, uint128 mod){
    uint128 result = {0, 1};
    int overflow = 0;

    base = modFun(base, mod, overflow); //efficient way
    while(e.hi > 0 || e.lo > 0){ //terminate once e.hi & lo = 0
        //check if odd
        if(e.lo & 1){
            //result = result * base % mod
            result = multiply(result, base, mod);
        }
        //e = e/2
        shiftRight(&e);

        //base = base * base % mod
        base = multiply(base, base, mod);
    }

    return result;
}

void reverse(char *str, int length) {
    int start = 0;
    int end = length - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

uint128 reverse_hex(uint128 num) {
    uint128 reversed;
    reversed.hi = 0;
    reversed.lo = 0;

    // Reverse bytes in the `hi` part
    for (int i = 0; i < 8; i++) {
        reversed.hi |= ((num.lo >> (i * 8)) & 0xFF) << ((7 - i) * 8);
    }

    // Reverse bytes in the `lo` part
    for (int i = 0; i < 8; i++) {
        reversed.lo |= ((num.hi >> (i * 8)) & 0xFF) << ((7 - i) * 8);
    }

    return reversed;
}



int main() {
/* Private-Key: (128 bit)                                                                                                                                         */
/* modulus: */
/*    00:e0:37:d3:5a:8b:16:0e:b7:f1:19:19:bf:ef:44: */
/*    09:17 */
/* publicExponent: 65537 (0x10001) */
/* privateExponent: */
/*    00:ca:b1:0c:ca:a4:43:7b:67:11:c9:77:a2:77:fe: */
/*    00:a1 */
/* prime1: 18125493163625818823 (0xfb8aafffd4b02ac7) */
/* prime2: 16442969659062640433 (0xe43129c94cf45f31) */
/* exponent1: 5189261458857000451 (0x4803f5cd8dcbfe03) */
/* exponent2: 12850891953204883393 (0xb2578a24fdb3efc1) */
/* coefficient: 10155582946292377246 (0x8cefe0e210c5a69e) */

    //DO NOT MODIFY
    uint128 modulus = {0xe037d35a8b160eb7LL,  0xf11919bfef440917LL};
    uint128 privateExp = {0x00cab10ccaa4437b67LL,  0x11c977a277fe00a1LL};
    uint64_t pubExp = 65537;
    const char plaintext[] = "Hello !";
    uint128 ciphertext;
    uint128 decrypted;
    //END DO NOT MODIFY

    /* YOUR CODE HERE: Implement RSA encryption, write the encrypted output of plaintext to ciphertext */
    uint128 message = {0, 0x21206f6c6c6548LL};
    uint128 exp = {0, pubExp};
    ciphertext = mod_exp(message, exp, modulus);

    //DO NOT MODIFY
    char *encrypted_text = (char*)&ciphertext;
    printf("encrypted=%s\n", encrypted_text);
    //END DO NOT MODIFY

    /* YOUR CODE HERE: Implement RSA decryption, write the decrypted output of ciphertext to decrypted */
    decrypted = mod_exp(ciphertext, privateExp, modulus);
    decrypted.hi = decrypted.lo;
    decrypted.lo = 0;


    //DO NOT MODIFY
    char *decrypted_text = (char*)&decrypted;
    printf("decrypted=%s\n", decrypted_text);
    assert(strcmp(plaintext, decrypted_text) == 0);
}
