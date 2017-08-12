#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "leptjson.h"

#define EXPECT(c, ch)             \
    do                            \
    {                             \
        assert(*c->json == (ch)); \
        c->json++;                \
    } while (0)

static void lept_parse_whitespace(lept_context *c)
{
    const char *p = c->json;
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\t')
        p++;
    c->json = p;
}

static int check_root_is_not_singular(lept_context *c, lept_value *v)
{
    lept_parse_whitespace(&c);
    if (*c->json != '\0')
    {
        v->type = LEPT_NULL;
        return LEPT_PARSE_ROOT_NOT_SINGULAR;
    }
    return LEPT_PARSE_OK;
}

static int lept_parse_literal(lept_context *c, lept_value *v, char *expect)
{
    EXPECT(c, expect[0]);
    int expect_word_len = strlen(expect);
    for (int i = 0; i < expect_word_len - 1; i++)
    {
        if (c->json[i] != expect[i + 1])
            return LEPT_PARSE_INVALID_VALUE;
    }
    c->json += expect_word_len - 1;
    switch (expect[0])
    {
    case 'n':
        v->type = LEPT_NULL;
        break;
    case 't':
        v->type = LEPT_TRUE;
        break;
    case 'f':
        v->type = LEPT_FALSE;
        break;
    }
    return LEPT_PARSE_OK;
}

static int lept_parse_number(lept_context *c, lept_value *v)
{
    char *end;
    /* \TODO validate number */
    v->n = strtod(c->json, &end);
    if (c->json == end)
        return LEPT_PARSE_INVALID_VALUE;
    c->json = end;
    v->type = LEPT_NUMBER;
    return LEPT_PARSE_OK;
}

static int lept_parse_value(lept_context *c, lept_value *v)
{
    switch (*c->json)
    {
    case 'n':
        return lept_parse_literal(c, v, "null");
    case 't':
        return lept_parse_literal(c, v, "true");
    case 'f':
        return lept_parse_literal(c, v, "false");
    case '\0':
        return LEPT_PARSE_EXPECT_VALUE;
    default:
        return lept_parse_number(c, v);
    }
}

int lept_parse(lept_value *v, const char *json)
{
    lept_context c;
    assert(v != NULL);
    c.json = json;
    v->type = LEPT_NULL;
    lept_parse_whitespace(&c);
    int result = lept_parse_value(&c, v);
    if (result == LEPT_PARSE_OK)
        result = check_root_is_not_singular(&c, v);
    return result;
}

lept_type lept_get_type(const lept_value *v)
{
    assert(v != NULL);
    return v->type;
}

double lept_get_number(const lept_value *v)
{
    assert(v != NULL && v->type == LEPT_NUMBER);
    return v->n;
}
