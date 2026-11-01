// TEST: capstone - sorting, binary search, function pointers, linked list
// FEATURES: everything above combined
// EXPECT: 189
struct Node
{
    int v;
    struct Node *next;
};

int add(int a, int b) { return a + b; }
int mul(int a, int b) { return a * b; }

int apply(int (*f)(int, int), int a, int b)
{
    return f(a, b);
}

void bubble_sort(int *a, int n)
{
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - 1 - i; j++)
            if (a[j] > a[j + 1])
            {
                int t = a[j];
                a[j] = a[j + 1];
                a[j + 1] = t;
            }
}

int bsearch_idx(int *a, int n, int key)
{
    int lo = 0;
    int hi = n - 1;
    while (lo <= hi)
    {
        int mid = lo + (hi - lo) / 2;
        if (a[mid] == key)
            return mid;
        if (a[mid] < key)
            lo = mid + 1;
        else
            hi = mid - 1;
    }
    return -1;
}

int list_sum(struct Node *n)
{
    int s = 0;
    while (n)
    {
        s += n->v;
        n = n->next;
    }
    return s;
}

int main()
{
    int data[8] = {42, 7, 19, 3, 88, 23, 11, 56};
    bubble_sort(data, 8); // 3 7 11 19 23 42 56 88

    struct Node c;
    c.v = 3;
    c.next = 0;
    struct Node b;
    b.v = 2;
    b.next = &c;
    struct Node a;
    a.v = 1;
    a.next = &b;

    return bsearch_idx(data, 8, 42)                // 5
           + (bsearch_idx(data, 8, 50) == -1) * 10 // 10
           + apply(add, data[0], data[7])          // 91
           + apply(mul, data[1], data[2])          // 77
           + list_sum(&a);                         // 6
}
