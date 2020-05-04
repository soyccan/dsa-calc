#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#define MAXN 1000000

struct node {
    int t;      // 0: null; 1: operand; 2: operator
    int v;      // value for int operand
    double w;   // value for double
    char s[4];  // repr for operator
    bool u;     // is unary
    struct node *l, *r;
};

struct node* root;

char opt[][4] = {"+",  "-",  "*", "/",  "%",  "&", "^",  "|",
                 "<<", ">>", "~", "&&", "||", "!", "==", "!="};
int optn = sizeof(opt) / sizeof(*opt);

char opt2[][4] = {"+", "-", "*", "sin", "cos", "exp", "log", "pow", "sqrt", "fabs"};
int opt2n = sizeof(opt2) / sizeof(*opt2);


int len;
double prob = 50;
double prob_grad = 0.999;  // bigger for longer expression

static inline int randint(int a, int b)
{
    return a + rand() / ((double) RAND_MAX / (b - a));
}

static inline double randdbl(double a, double b)
{
    return a + rand() / ((double) RAND_MAX / (b - a));
}

void build(struct node** pnd, int lv, int val)
{
    bool b = rand() & 1;
    b&& putchar('(');

    struct node* nd;
    *pnd = nd = malloc(sizeof(struct node));

    nd->t = ((rand() % (int) (prob + 1)) != 0) + 1;
    prob *= prob_grad;
    if (nd->t == 1) {
        if (val)
            nd->v = val;
        else
            nd->v = randint(-1000, 1000);
        printf("%d", nd->v);
    } else {  // == 2
        strcpy(nd->s, opt[rand() % optn]);

        char c = nd->s[0];
        char d = nd->s[1];
        bool maybe = c == '+' || c == '-';
        bool unary = (c == '!' && d == 0) || c == '~';

        if ((!unary && !maybe) || (maybe && (rand() & 1))) {
            // binary
            nd->u = false;
            build(&nd->l, lv + 1, 0);
            printf("%s", nd->s);

            // make it less likely to overflow on shifting
            int val = 0;
            if (*nd->s == '<' || *nd->s == '>')
                val = rand() % 7;
            build(&nd->r, lv + 1, val);
        } else {
            nd->u = true;
            printf("%s", nd->s);
            build(&nd->l, lv + 1, 0);
        }
    }
    b&& putchar(')');
}

void build2(struct node** pnd, int lv, double val)
{
    bool b = rand() & 1;
    b&& putchar('(');

    struct node* nd;
    *pnd = nd = malloc(sizeof(struct node));

    nd->t = ((rand() % (int) (prob + 1)) != 0) + 1;
    prob *= prob_grad;
    if (nd->t == 1) {
        if (val)
            nd->w = val;
        else
            nd->w = randdbl(-1000, 1000);
        printf("%f", nd->w);
    } else {  // == 2
        strcpy(nd->s, opt2[rand() % opt2n]);

        char c = nd->s[0];
        bool maybe = c == '+' || c == '-';
        bool func = c == 's' || c == 'c' || c == 'e' || c == 'l' || c == 'f' || c == 'p';

        if (func) {
            // function
            printf("%s(", nd->s);
            build2(&nd->l, lv + 1, 0);
            if (c == 'p') {
                printf(",");
                build2(&nd->r, lv + 1, 0);
            }
            putchar(')');
        } else if (!maybe || (rand() & 1)) {
            // binary
            nd->u = false;
            build2(&nd->l, lv + 1, 0);
            printf("%s", nd->s);
            build2(&nd->r, lv + 1, 0);
        } else {
            // unary
            nd->u = true;
            printf("%s", nd->s);
            build2(&nd->l, lv + 1, 0);
        }
    }
    b&& putchar(')');
}

int main(int argc, char** argv)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    srand((tv.tv_sec & 2) + tv.tv_usec);
    if (argv[1][0] == '1')
        build(&root, 0, 0);
    else
        build2(&root, 0, 0);
    return 0;
}