#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

void *box(void *object, size_t size)
{
    char *ptr = malloc(size);
    if (ptr == NULL)
    {
        exit(1);
    }
    memcpy(ptr, object, size);
    return ptr;
}

#define BOX(x) box(&x, sizeof(x))

typedef struct Span
{
    int start, end;
} Span;

// Expr value types
typedef struct String
{
} String;

typedef struct Integer
{
} Integer;

typedef struct BinOp
{
    struct Expr *left;
    struct Expr *right;
} BinOp;

// Expr
typedef enum ExprTy
{
    ExprTy_String,
    ExprTy_Integer,
    ExprTy_Pow,
    ExprTy_Mul,
    ExprTy_Div,
    ExprTy_Add,
    ExprTy_Sub,
} ExprTy;

typedef union ExprVal
{
    String string;
    Integer integer;
    BinOp pow;
    BinOp mul;
    BinOp div;
    BinOp add;
    BinOp sub;
} ExprVal;

typedef struct Expr
{
    ExprTy ty;
    Span span;
    ExprVal val;
} Expr;

void write_Span(char *s, Span span)
{
    fwrite(s + span.start, 1, span.end - span.start, stdout);
}

void display_Expr(Expr *expr, char *s, int level)
{
    for (int i = 0; i < level; ++i)
    {
        printf("  ");
    }
    switch (expr->ty)
    {
    case ExprTy_String:
        printf("String(\"");
        write_Span(s, expr->span);
        printf("\")\n");
        break;

    case ExprTy_Integer:
        break;

    case ExprTy_Pow:
        printf("Pow\n");
        display_Expr(expr->val.pow.left, s, level + 1);
        display_Expr(expr->val.pow.right, s, level + 1);
        break;

    case ExprTy_Mul:
        printf("Mul\n");
        display_Expr(expr->val.mul.left, s, level + 1);
        display_Expr(expr->val.mul.right, s, level + 1);
        break;

    case ExprTy_Div:
        printf("Div\n");
        display_Expr(expr->val.div.left, s, level + 1);
        display_Expr(expr->val.div.right, s, level + 1);
        break;
    }
}

void destroy_Expr(Expr *expr)
{
    switch (expr->ty)
    {
    case ExprTy_Integer:
        break;
    case ExprTy_String:
        break;
    case ExprTy_Pow:
        destroy_Expr(expr->val.pow.left);
        free(expr->val.pow.left);
        destroy_Expr(expr->val.pow.right);
        free(expr->val.pow.right);
        break;
    }
}

typedef struct ResultExpr
{
    bool valid;
    Expr expr;
} ResultExpr;

void destroy_ResultExpr(ResultExpr *result)
{
    if (result->valid)
    {
        destroy_Expr(&result->expr);
    }
}

/// @brief String
///     '"' (!'"')* '"'
/// @param s
/// @param i
ResultExpr parse_String(char *s, int i)
{
    ResultExpr result;
    result.expr.span.start = i;
    if (s[i++] == '"')
    {
        while (s[i] != '\0' && s[i] != '"')
        {
            ++i;
        }
        if (s[i] == '\0')
        {
            printf("\x1b[1;31merror:\x1b[0m unfinished string\n");
            result.valid = false;
            return result;
        }
        if (s[i] == '"')
        {
            ++i;
            result.expr.ty = ExprTy_String;
            result.expr.span.end = i;
            result.expr.val.string;
            result.valid = true;
            return result;
        }
    }
    return result;
}

void test_parse_String()
{
    char *s = "\"Valid string\"";
    ResultExpr result1 = parse_String(s, 0);
    assert(result1.valid);
    assert(result1.expr.ty == ExprTy_String);

    // Should fail with "error: unfinished string"
    // ResultExpr result2;
    // result2 = parse_String("\"", 0);
}

/// @brief Parse s to match a token m with whitespace surrounding it. Returns -1 if the parse fails. Otherwise, returns the final index.
/// @param s String to parse from
/// @param i Index into the string at which to start
/// @param m String to match
/// @return -1 if the parse fails, otherwise the index of one past the end
int token_spaced(char *s, int i, char *m)
{
    int len = strlen(m);
    while (s[i] == ' ' || s[i] == '\n' || s[i] == '\r' || s[i] == '\b' || s[i] == '\t')
    {
        ++i;
    }
    int j = 0;
    while (m[j] == s[i + j] && m[j] != '\0' && s[i + j] != '\0')
    {
        ++j;
    }
    if (j != len)
    {
        return -1;
    }
    i += j;
    while (s[i] == ' ' || s[i] == '\n' || s[i] == '\r' || s[i] == '\b' || s[i] == '\t')
    {
        ++i;
    }
    return i;
}

void test_token_spaced()
{
    assert(token_spaced("  *  ", 1, "*") == 5);
    assert(token_spaced("  ^  ", 1, "*") == -1);
    assert(token_spaced("  *", 1, "*") == 3);
    assert(token_spaced("    ", 1, "*") == -1);
}

ResultExpr parse_Pow(char *s, int i)
{
    ResultExpr out, left, right;
    out.expr.span.start = i;

    // Get left
    left = parse_String(s, i);
    if (!left.valid)
    {
        out.valid = false;
        return out;
    }
    i = left.expr.span.end;
    i = token_spaced(s, i, "^");
    if (i == -1)
    {
        return left;
    }

    // Get right
    right = parse_Pow(s, i);
    if (!right.valid)
    {
        return left;
    }
    i = right.expr.span.end;

    out.valid = true;
    out.expr.ty = ExprTy_Pow;
    out.expr.span.end = i;

    out.expr.val.pow.left = BOX(left.expr);
    out.expr.val.pow.right = BOX(right.expr);
    return out;
}

void test_parse_Pow()
{
    ResultExpr result1;
    result1 = parse_Pow("\"x\" ^ \"y\"", 0);
    assert(result1.valid);
    assert(result1.expr.ty == ExprTy_Pow);

    BinOp pow1 = result1.expr.val.pow;
    assert((pow1.left)->ty == ExprTy_String);
    assert((pow1.right)->ty == ExprTy_String);
    assert(result1.expr.span.start == 0);
    assert(result1.expr.span.end == 9);

    ResultExpr result2;
    result2 = parse_Pow("\"x\" ^ \"y\" ^ \"z\"", 0);
    assert(result2.valid);
    assert(result2.expr.ty == ExprTy_Pow);

    BinOp pow2 = result2.expr.val.pow;
    assert((pow2.left)->ty == ExprTy_String);
    assert((pow2.right)->ty == ExprTy_Pow);
    assert(result2.expr.span.start == 0);
    assert(result2.expr.span.end == 15);

    ResultExpr result3;
    result3 = parse_Pow("\"x\"", 0);
    assert(result3.valid);
    assert(result3.expr.ty == ExprTy_String);
    assert(result3.expr.span.end == 3);
}

ResultExpr parse_Mul(char *s, int i)
{
    int start = i;
    ResultExpr left_res = parse_Pow(s, i);
    if (!left_res.valid)
    {
        return left_res;
    }
    Expr left = left_res.expr;
    i = left.span.end;

    while (true)
    {
        int backup = i;
        ResultExpr right;
        if ((i = token_spaced(s, i, "*")) != -1)
        {
            // Get a pow
            right = parse_Pow(s, i);
            if (!right.valid)
            {
                // Since we already have a valid *, we expect a rhs expression
                printf("\x1b[1;31merror:\x1b[0m bad mul expr\n");
                exit(1);
            }
            i = right.expr.span.end;
            left.val.mul.left = BOX(left);
            left.val.mul.right = BOX(right.expr);
            left.ty = ExprTy_Mul;
            left.span.start = start;
            left.span.end = i;
        }
        else if ((i = token_spaced(s, backup, "/")) != -1)
        {
            right = parse_Pow(s, i);
            if (!right.valid)
            {
                // Since we already have a valid *, we expect a rhs expression
                printf("\x1b[1;31merror:\x1b[0m bad div expr\n");
                exit(1);
            }
            i = right.expr.span.end;
            left.val.div.left = BOX(left);
            left.val.div.right = BOX(right.expr);
            left.ty = ExprTy_Div;
            left.span.start = start;
            left.span.end = i;
        }
        else
        {
            break;
        }
    }

    ResultExpr out;
    out.valid = true;
    out.expr = left;
    return out;
}

void test_parse_Mul()
{
    char *s1 = "\"a\" * \"b\" ^ \"c\" * \"d\"";
    ResultExpr r1 = parse_Mul(s1, 0);
    assert(r1.valid);
    // display_Expr(&r1.expr, s, 0);
    // Mul
    //   Mul
    //     String(""a"")
    //     Pow
    //       String(""b"")
    //       String(""c"")
    //   String(""d"")
    assert(r1.expr.ty == ExprTy_Mul);

    Expr left = *r1.expr.val.mul.left;
    assert(left.ty == ExprTy_Mul);
    assert(left.val.mul.left->ty == ExprTy_String);
    assert(left.val.mul.right->ty == ExprTy_Pow);

    char *s2 = "\"a\" * \"b\" ^ \"c\" / \"d\"";
    ResultExpr r2 = parse_Mul(s2, 0);
    assert(r2.valid);
    // display_Expr(&r2.expr, s, 0);
    assert(r2.expr.ty == ExprTy_Div);

    Expr left2 = *r2.expr.val.div.left;
    assert(left2.ty == ExprTy_Mul);
    assert(left2.val.mul.left->ty == ExprTy_String);
    assert(left2.val.mul.right->ty == ExprTy_Pow);
}

ResultExpr parse_Add(char *s, int i)
{
    int start = i;
    ResultExpr left_res = parse_Mul(s, i);
    if (!left_res.valid)
    {
        return left_res;
    }
    Expr left = left_res.expr;
    i = left.span.end;

    while (true)
    {
        int backup = i;
        ResultExpr right;
        if ((i = token_spaced(s, i, "+")) != -1)
        {
            // Get a mul
            right = parse_Mul(s, i);
            if (!right.valid)
            {
                // Since we already have a valid +, we expect a rhs expression
                printf("\x1b[1;31merror:\x1b[0m bad add expr\n");
                exit(1);
            }
            i = right.expr.span.end;
            left.val.add.left = BOX(left);
            left.val.add.right = BOX(right.expr);
            left.ty = ExprTy_Add;
            left.span.start = start;
            left.span.end = i;
        }
        else if ((i = token_spaced(s, backup, "-")) != -1)
        {
            right = parse_Mul(s, i);
            if (!right.valid)
            {
                // Since we already have a valid -, we expect a rhs expression
                printf("\x1b[1;31merror:\x1b[0m bad sub expr\n");
                exit(1);
            }
            i = right.expr.span.end;
            left.val.sub.left = BOX(left);
            left.val.sub.right = BOX(right.expr);
            left.ty = ExprTy_Sub;
            left.span.start = start;
            left.span.end = i;
        }
        else
        {
            break;
        }
    }

    ResultExpr out;
    out.valid = true;
    out.expr = left;
    return out;
}

void test_parse_Add()
{
    char *s1 = "\"a\" + \"b\" * \"c\" + \"d\"";
    ResultExpr r1 = parse_Add(s1, 0);
    assert(r1.valid);
    // display_Expr(&r1.expr, s, 0);
    assert(r1.expr.ty == ExprTy_Add);

    Expr left = *r1.expr.val.add.left;
    assert(left.ty == ExprTy_Add);
    assert(left.val.add.left->ty == ExprTy_String);
    assert(left.val.add.right->ty == ExprTy_Mul);

    char *s2 = "\"a\" + \"b\" * \"c\" - \"d\"";
    ResultExpr r2 = parse_Add(s2, 0);
    assert(r2.valid);
    // display_Expr(&r2.expr, s, 0);
    assert(r2.expr.ty == ExprTy_Sub);

    Expr left2 = *r2.expr.val.sub.left;
    assert(left2.ty == ExprTy_Add);
    assert(left2.val.add.left->ty == ExprTy_String);
    assert(left2.val.add.right->ty == ExprTy_Mul);
}

int main()
{
    test_parse_String();
    test_token_spaced();
    test_parse_Pow();
    test_parse_Mul();
    test_parse_Add();
}
