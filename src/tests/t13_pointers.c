// TEST: address-of, dereference, pointer-to-pointer, out-params
// FEATURES: & * ** pointer args
// EXPECT: 23
void swap(int *a, int *b)
{
    int t = *a;
    *a = *b;
    *b = t;
}

int main()
{
    int x = 10;
    int *p = &x;
    *p = *p + 5;
    int **pp = &p;
    **pp += 2; // x == 17

    int a = 3;
    int b = 9;
    swap(&a, &b);     // a == 9, b == 3
    return x + a - b; // 17 + 6
}
