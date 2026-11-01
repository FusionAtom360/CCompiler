// TEST: global variables and function-local statics
// FEATURES: .data/.bss, static storage duration
// EXPECT: 36
int counter = 0;
int base = 20;

void bump()
{
    counter += 2;
}

int next()
{
    static int n = 0;
    n++;
    return n;
}

int main()
{
    for (int i = 0; i < 5; i++)
        bump(); // counter == 10
    int a = next();
    int b = next();
    int c = next(); // 1 + 2 + 3
    return counter + a + b + c + base - 20 + 20;
}
