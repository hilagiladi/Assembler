
#include "assembler.h"
#include "tables.h"
#include "utils.h"
#include "word.h"
#include "constants.h"


void changeBits(Word *w, unsigned int start, unsigned int end, int newValue) {
    int size, mask, mask2;
    unsigned int v;
    size = end - start + 1;

    mask = -1;  /*mask = 11...1*/
    mask = mask << (end + 1);  /*mask = 1...1 0...0 (number of '0' is to)*/

    mask2 = 1;
    mask2 = mask2 << start;  /*mask2 = 0...0 1 0...0*/
    mask2 = mask2 - 1;  /*mask2 = 0...0 1...1*/

    mask = mask | mask2;      /*mask = 1...1 0...0 1...1 (the zeros are at "from-to")*/
    w->value = w->value & mask;  /*zeros at [start..end] bits*/

    /* make value only on 'size' bits:*/
    v = (unsigned int) newValue;
    v <<= (10 - size); /* remove all the bits that are beyond 'size' */
    v >>= (9 - end); /* put the value bits in [start..end] */

    w->value = w->value | v;  /*put the value v in bits [start...end] without changing any of the other bits*/

}

int getBits(Word w, unsigned int start, unsigned int end) {
    Word temp;
    temp.value = w.value;
    temp.value = temp.value << (9 - end);  /*number of bits to the left*/
    temp.value = temp.value >> (10 - (end - start + 1));  /*move the bits to the right*/
    return (int) temp.value;
}
