// TEST: local arrays, indexing, pointer arithmetic
// FEATURES: arrays, [], pointer + int, decay
// EXPECT: 149
int main()
{
    int a[8];
    for (int i = 0; i < 8; i++)
        a[i] = i * i;
    int sum = 0;
    for (int i = 0; i < 8; i++)
        sum += a[i];       // 140
    return sum + *(a + 3); // + 9
}
