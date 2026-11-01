// TEST: while loop
// FEATURES: loops, compound assignment
// EXPECT: 55
int main()
{
    int i = 1;
    int sum = 0;
    while (i <= 10)
    {
        sum += i;
        i++;
    }
    return sum;
}
