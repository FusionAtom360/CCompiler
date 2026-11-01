// TEST: local variables with initializers
// FEATURES: declarations, assignment, stack slots
// EXPECT: 32
int main()
{
    int a = 5;
    int b = 7;
    int c;
    c = a * b;
    return c - 3;
}
