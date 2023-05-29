#ifndef __ERR_H__
#define __ERR_H__

#include "box.h"
#include "parsing.h"

// Parse error node
typedef struct
{
    Span span;
    char *message;
    ParseErrors *next;
} ParseError;

// Singly-linked list of parse errors
// In order to use, simply declare
//   ParseErrors errors;
// Then pass this into whatever parser with &
//   if (parse(s, &errors)) { ... }
typedef ParseError *ParseErrors;

// Prepend an error to the list
void push_ParseError(Span span, char *message, ParseErrors *errors)
{
    ParseError out;
    out.span = span;
    out.message = message;
    out.next = *errors;
    *errors = (ParseErrors)BOX(out);
}

// Destroy ParseErrors
void destroy_ParseErrors(ParseErrors errors)
{
    if (errors != NULL)
    {
        destroy_ParseErrors(errors->next);
        free(errors);
    }
}

#endif
