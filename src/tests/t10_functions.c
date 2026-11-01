// TEST: function calls, forward declaration, nested calls
// FEATURES: calling convention, prototypes
// EXPECT: 33
int square(int x);

int sub(int a, int b)
{
    return a - b;
}

int main()
{
    return sub(square(7), square(4));
}

int square(int x)
{
    return x * x;
}
