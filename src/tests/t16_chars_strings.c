// TEST: char type, string literals, char arrays
// FEATURES: char, 'x' literals, "..." literals, byte loads/stores
// EXPECT: 125
int my_strlen(char *s)
{
    int n = 0;
    while (s[n])
        n++;
    return n;
}

int is_vowel(char c)
{
    return c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u';
}

int count_vowels(char *s)
{
    int n = 0;
    int i = 0;
    while (s[i])
    {
        n += is_vowel(s[i]);
        i++;
    }
    return n;
}

int main()
{
    char w[3];
    w[0] = 'o';
    w[1] = 'k';
    w[2] = 0;
    return my_strlen("hello, world") * 10 + count_vowels("compiler test") + (my_strlen(w) == 2);
}
