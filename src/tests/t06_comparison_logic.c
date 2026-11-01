// TEST: relational, equality, logical operators
// FEATURES: < > <= >= == != && ||
// EXPECT: 53
int main()
{
    int a = 3;
    int b = 5;
    return (a < b) + (a > b) * 2 + (a == 3) * 4 + (b != 5) * 8 + (a <= 3 && b >= 5) * 16 + (a > 5 || b > 4) * 32;
}
