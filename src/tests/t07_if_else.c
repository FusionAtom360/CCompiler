// TEST: if / else if / else, nested
// FEATURES: branching, labels
// EXPECT: 22
int main()
{
    int x = 15;
    int r = 0;
    if (x < 10)
    {
        r = 1;
    }
    else if (x < 20)
    {
        r = 2;
        if (x % 5 == 0)
            r = r + 20;
    }
    else
    {
        r = 3;
    }
    return r;
}
