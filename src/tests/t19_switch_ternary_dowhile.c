// TEST: switch with fallthrough/default, ternary, do-while
// FEATURES: jump tables or compare chains, ?:
// EXPECT: 122
int f(int n)
{
    int r = 0;
    switch (n)
    {
    case 1:
        r += 1; // falls through
    case 2:
        r += 2;
        break;
    case 3:
        r += 4;
        break;
    default:
        r += 8;
    }
    return r;
}

int main()
{
    int total = f(1) + f(2) + f(3) + f(9); // 3 + 2 + 4 + 8 = 17
    int i = 0;
    do
    {
        i++;
    } while (i < 5);
    return total + i + (i > 3 ? 100 : 0); // 17 + 5 + 100
}
