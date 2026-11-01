// TEST: for loop with break and continue
// FEATURES: for, break, continue
// EXPECT: 25
int main()
{
    int s = 0;
    for (int i = 0; i < 100; i++)
    {
        if (i % 2 == 0)
            continue;
        if (i > 9)
            break;
        s += i;
    }
    return s;
}
