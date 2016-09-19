//----------------------------------------------------------------
// Zadanie: Wskazówki
// Autor: Tomasz Kacperek <tk334578@students.mimuw.edu.pl>
//----------------------------------------------------------------

#include <cstdio>
using namespace std;

//----------------------------------------------------------------
// STRUKTURY
//----------------------------------------------------------------

// opis wskazówki lub złożenia wielu wskazówek
struct transformation
{
    // wektor przemieszczenia
    long x, y;
    // zwrot po zakończeniu operacji
    // przy założeniu, że przed transformacją profesor patrzył na północ
    char dir;

    transformation(long x, long y, char dir) : x(x), y(y), dir(dir) {}
    transformation(transformation const & t) : x(t.x), y(t.y), dir(t.dir) {}

    transformation(char c, long d) : x(0), y(0), dir(0)
    {
        switch (c)
        {
            case  'L':
                x = -d, dir = 3;
                break;
            case  'R':
                x = d, dir = 1;
                break;
            case  'U':
                y = -d, dir = 2;
                break;
            default: {}
        }
    }

    transformation & operator=(transformation const & t)
    {
        x = t.x, y = t.y, dir = t.dir;
        return * this;
    }
};

// zerowa transformacja
static const transformation zero = transformation(0, 0, 0);

// węzeł w drzewie
struct node
{
    // rodzic i dzieci
    node * top, * left, * right;

    // rozmiar drzewa
    long size;
    // współczynnik równowagi
    // = hr - hl
    int bf;
    // bazowa transformacja, złożenie transformacji z poddrzewa
    transformation base, comb;

    node(transformation const & base, long size);
};

// pusty node
node nullNodeObj = node(zero, 0);
node * nullNode = &nullNodeObj;

// konstruktor node
node::node(transformation const & base, long size = 1)
    : top(nullNode), left(nullNode), right(nullNode),
        size(size), bf(0), base(base), comb(base) {}

//----------------------------------------------------------------
// FUNKCJE
//----------------------------------------------------------------

// złożenie transformacji
void compose(transformation & ret,
        transformation const & t1, transformation const & t2)
{
    ret = t1;
    switch (t1.dir)
    {
        case 0:
            ret.x += t2.x, ret.y += t2.y;
            break;
        case 1:
            ret.x += t2.y, ret.y -= t2.x;
            break;
        case 2:
            ret.x -= t2.x, ret.y -= t2.y;
            break;
        case 3:
            ret.x -= t2.y, ret.y += t2.x;
            break;
        default: {}
    }
    ret.dir = (ret.dir + t2.dir) % 4;
}

// 3-argumentowe złożenie
inline void
compose(transformation & ret,
        transformation const & t1,
        transformation const & t2,
        transformation const & t3)
{
    compose(ret, t1, t2);
    compose(ret, ret, t3);
}

// oblicza comb
inline void countCombOf(node * n)
{
     compose(n->comb, n->left->comb, n->base, n->right->comb);
}

// oblicza size
inline void countSizeOf(node * n)
{
    n->size = 1 + n->left->size + n->right->size;
}

// zwraca referencję do lewego dziecka
inline node * & left(node * n)
{
    return n->left;
}

// zwraca referencję do lewego dziecka
inline node * & right(node * n)
{
    return n->right;
}

// uniwersalna implementacja rotacji RR i LL
template <node * & first(node *), node * & second(node *), int side>
node * rotateAA(node * & root, node * n)
{
    node * top = n->top, * A = n, * B = first(n);

    // ustawianie wskaźników

    first(A) = second(B);
    if (first(A)->size)
        first(A)->top = A;

    second(B) = A;
    A->top = B;

    B->top = top;
    if (top->size)
    {
        if (second(top) == n)
            second(top) = B;
        else
            first(top) = B;
    }
    else
        root = B;

    // ustawianie parametrów

    if (B->bf == side)
        A->bf = B->bf = 0;
    else
        A->bf = side, B->bf = 0;

    B->size = A->size;
    countSizeOf(A);

    B->comb = A->comb;
    countCombOf(A);

    return B;
}

node * (* rotateRR)(node * &, node *) = & rotateAA<right, left, 1>;
node * (* rotateLL)(node * &, node *) = & rotateAA<left, right, -1>;

// uniwersalna implementacja rotacji RL i LR
template <node * & first(node *), node * & second(node *), int side>
node * rotateAB(node * & root, node * n)
{
    node * top = n->top, * A = n, * B = first(n), * C = second(B);

    // ustawianie wskaźników

    first(A) = second(C);
    if (first(A)->size)
        first(A)->top = A;
    A->top = C;
    second(C) = A;

    second(B) = first(C);
    if (second(B)->size)
        second(B)->top = B;
    B->top = C;
    first(C) = B;

    C->top = top;
    if (top->size)
    {
        if (first(top) == n)
            first(top) = C;
        else
            second(top) = C;
    }
    else
        root = C;

    // ustawianie parametrów

    switch (C->bf)
    {
        case side:
            A->bf = -side, B->bf = 0;
            break;
        case -side:
            A->bf = 0,     B->bf = side;
            break;
        case 0:
            A->bf = 0,     B->bf = 0;
            break;
        default: {}
    }
    C->bf = 0;


    C->size = A->size;
    countSizeOf(A);
    countSizeOf(B);

    C->comb = A->comb;
    countCombOf(A);
    countCombOf(B);

    return C;
}

node * (* rotateRL)(node * &, node *) = & rotateAB<right, left, 1>;
node * (* rotateLR)(node * &, node *) = & rotateAB<left, right, -1>;

// wstawianie do drzewa
void insert(node * & root, const transformation & trans, long pos,
                      transformation & answer)
{
    answer = zero;
    node * newNode = new node(trans);

    if (!root->size)
        root = newNode;
    else
    {
        node * up, * p = root;
        bool found = false;
        // pos mówi, ile wskazówek w drzewie poprzedza nową
        --pos;

        // szukanie odpowiedniego miejsca
        while (!found)
        {
            // idź w prawo
            long count = p->left->size + 1;
            if (count <= pos)
            {
                compose(answer, answer, p->left->comb, p->base);

                if (p->right->size)
                {
                    p = p->right;
                    pos -= count;
                }
                else
                {
                    p->right = newNode;
                    found = true;
                }
            }
            // idź w lewo
            else
            {
                if (p->left->size)
                    p = p->left;
                else
                {
                    p->left = newNode;
                    found = true;
                }
            }
        }

        newNode->top = p;
        up = p;
        p = newNode;

        // aktualizacja drzewa i balansowanie
        bool rotated = false, clearedBf = false;
        while (up->size)
        {
            up->size++;
            countCombOf(up);

            if (!(clearedBf || rotated))
            {
                up->bf += (up->left == p) ? -1 : 1;
                if (up->bf == 0)
                    clearedBf = true;
                else
                {
                    rotated = true;
                    if (up->bf == -2)
                        up = (p->bf <= 0) ? rotateLL(root, up) : rotateLR(root, up);
                    else if (up->bf == 2)
                        up = (p->bf >= 0) ? rotateRR(root, up) : rotateRL(root, up);
                    else
                        rotated = false;
                }
            }
            p = up;
            up = up->top;
        }
    }
    compose(answer, answer, trans);
}

// usuwanie drzewa
void eraseTree(node * root)
{
    node * h, * p = root;
    while (p->size)
    {
        if (p->left->size)
            p = p->left;
        else if (p->right->size)
            p = p->right;
        else
        {
            h = p->top;
            if (h->size)
            {
                if (p == h->left)
                    h->left = nullNode;
                else
                    h->right = nullNode;
            }
            delete p;
            p = h;
        }
    }
}

int main()
{
    long n, k, d;
    char c;
    node * root = nullNode;
    transformation ans(zero);

    scanf("%ld", & n);
    for (long i = 0; i < n; ++i)
    {
        scanf("%ld %c %ld", & k, & c, & d);
        insert(root, transformation(c, d), k, ans);
        printf("%ld %ld\n", ans.x, ans.y);
    }
    eraseTree(root);
}
