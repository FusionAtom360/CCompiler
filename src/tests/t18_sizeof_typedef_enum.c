// TEST: sizeof, typedef, enum, unsigned char wraparound
// FEATURES: type system; assumes LP64 (int=4, long=8)
// EXPECT: 28
typedef unsigned char u8;
enum Color
{
    RED,
    GREEN = 5,
    BLUE
};

int main()
{
    u8 x = 250;
    x += 10;                                         // wraps to 4
    return sizeof(int) + sizeof(char) + sizeof(long) // 13
           + BLUE + GREEN                            // 6 + 5
           + x;                                      // 4
}
