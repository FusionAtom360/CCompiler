// TEST: unary and bitwise operators
// FEATURES: - ~ ! & | ^ <<
// EXPECT: 25
int main()
{
    int a = 6;
    int b = -a;
    (void)b;
    return (((~a & 0xF) | (a << 2)) ^ 1) + !(a - 6);
}
