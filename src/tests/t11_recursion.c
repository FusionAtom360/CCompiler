// TEST: recursion (fib, gcd)
// FEATURES: recursive calls, stack frames
// EXPECT: 61
int fib(int n)
{
    if (n < 2)
        return n;
    return fib(n - 1) + fib(n - 2);
}

int gcd(int a, int b)
{
    if (b == 0)
        return a;
    return gcd(b, a % b);
}

int main()
{
    return fib(10) + gcd(48, 18);
}
