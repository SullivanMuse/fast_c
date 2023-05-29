#ifndef __PARSING_H__
#define __PARSING_H__

#include <stdbool.h>

typedef struct Input
{
    char *s;
    int i;
} Input;

// Parsers return bool
// If the bool is false, the parser will not have changed

// Current character
// '\0' if at end of string
char curr(Input *s)
{
    return s->s[s->i];
}

// Move parser forward one character
// If returns false, does not advance the parser (end of input)
bool advance(Input *s)
{
    if (curr(s) != '\0')
    {
        ++s->i;
        return true;
    }
    else
    {
        return false;
    }
}

// Consume a character
// If returns '\0', does not advance the input (end of input)
char take(Input *s)
{
    char c = curr(s);
    advance(s);
    return c;
}

// Parse zero or more
// p*
// Returns true iff the parse succeeds
// Does not consume input if the parse fails
bool many0(bool p(Input *), Input *s)
{
    while (p(s))
    {
    }
    return true;
}

// Parse one or more
// p+ = p p*
// Returns true iff the parse succeeds
// Does not consume input if the parse fails
bool many1(bool p(Input *), Input *s)
{
    if (p(s))
    {
        many0(p, s);
        return true;
    }
    else
    {
        return false;
    }
}

// Parse an exact match
// m
// Returns true iff the parse succeeds
// Does not consume input if the parse fails
bool tag(char *m, Input *s)
{
    int offset = 0;
    while (s->s[s->i + offset] == m[offset] && m[offset] != '\0' && s->s[s->i + offset] != '\0')
    {
        ++offset;
    }
    if (m[offset] == '\0')
    {
        s->i += offset;
        return true;
    }
    else
    {
        return false;
    }
}

// Parse complement
// !p
// Does not consume input
// Returns true iff the parse fails
bool complement(bool p(Input *), Input *s)
{
    int i = s->i;
    if (p(s))
    {
        s->i = i;
        return false;
    }
    else
    {
        return true;
    }
}

// Parse one digit
// '0'..='9'
// Returns true iff the parse succeeds
// Does not consume input if the parse fails
bool digit(Input *s)
{
    if ('0' <= curr(s) && curr(s) <= '9')
    {
        ++s->i;
        return true;
    }
    else
    {
        return false;
    }
}

// Parse zero or more digits
// ('0'..='9')*
// Returns true iff the parse succeeds
// Does not consume input if the parse fails
bool digit0(Input *s)
{
    return many0(digit, s);
}

// Parse one or more digits
// ('0'..='9')+
// Returns true iff the parse succeeds
// Does not consume input if the parse fails
bool digit1(Input *s)
{
    return many1(digit, s);
}

// Denotes portion of the input
typedef struct Span
{
    int start, end;
} Span;

// Set start of span
void span_start(Input *s, Span *span)
{
    span->start = s->i;
}

// Set end of span
void span_end(Input *s, Span *span)
{
    span->end = s->i;
}

#endif __PARSING_H__
