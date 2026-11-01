// TEST: structs, nested structs, ., ->, by-value copy and pass
// FEATURES: struct layout, member offsets, struct copy
// EXPECT: 55
struct Point
{
    int x;
    int y;
};
struct Rect
{
    struct Point tl;
    struct Point br;
};

int dist2(struct Point *p)
{
    return p->x * p->x + p->y * p->y;
}

int area(struct Rect r)
{
    return (r.br.x - r.tl.x) * (r.br.y - r.tl.y);
}

int main()
{
    struct Point p;
    p.x = 3;
    p.y = 4;

    struct Point q = p; // copy
    q.y = 0;            // must not affect p

    struct Rect r;
    r.tl.x = 1;
    r.tl.y = 2;
    r.br.x = 6;
    r.br.y = 8;

    return area(r) + dist2(&p) + q.y; // 30 + 25 + 0
}
