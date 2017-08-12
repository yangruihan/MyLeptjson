#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h> /* errno, ERANGE */
#include <math.h>  /* HUGE_VAL */
#include "leptjson.h"

#define EXPECT(c, ch)             \
    do                            \
    {                             \
        assert(*c->json == (ch)); \
        c->json++;                \
    } while (0)

#define ISDIGIT(ch) ((ch) >= '0' && (ch) <= '9')
#define ISDIGIT1TO9(ch) ((ch) >= '1' && (ch) <= '9')

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

static int lept_parse_literal(lept_context *c, lept_value *v, char *expect, lept_type type)
{
    EXPECT(c, expect[0]);
    // int expect_word_len = strlen(expect);
    // for (int i = 0; i < expect_word_len - 1; i++)
    // {
    //     if (c->json[i] != expect[i + 1])
    //         return LEPT_PARSE_INVALID_VALUE;
    // }
    // c->json += expect_word_len - 1;
    size_t i;
    for (i = 0; expect[i + 1]; i++)
        if (c->json[i] != expect[i + 1])
            return LEPT_PARSE_INVALID_VALUE;
    c->json += i;
    v->type = type;
    return LEPT_PARSE_OK;
}

static int lept_parse_number(lept_context *c, lept_value *v)
{
    const char *p = c->json;

    if (*p == '-')
        p++;

    if (*p == '0')
        p++;
    else
    {
        if (!ISDIGIT1TO9(*p))
            return LEPT_PARSE_INVALID_VALUE;
        for (p++; ISDIGIT(*p); p++)
            ;
    }

    if (*p == '.')
    {
        p++;
        if (!ISDIGIT(*p))
            return LEPT_PARSE_INVALID_VALUE;
        for (p++; ISDIGIT(*p); p++)
            ;
    }

    if (*p == 'e' || *p == 'E')
    {
        p++;
        if (*p == '+' || *p == '-')
            p++;
        if (!ISDIGIT(*p))
            return LEPT_PARSE_INVALID_VALUE;
        for (p++; ISDIGIT(*p); p++)
            ;
    }

    errno = 0;
    v->n = strtod(c->json, NULL);
    if (errno == ERANGE && (v->n == HUGE_VAL || v->n == -HUGE_VAL))
        return LEPT_PARSE_NUMBER_TOO_BIG;
    v->type = LEPT_NUMBER;
    c->json = p;
    return LEPT_PARSE_OK;
}

static int lept_parse_value(lept_context *c, lept_value *v)
{
    switch (*c->json)
    {
    case 'n':
        return lept_parse_literal(c, v, "null", LEPT_NULL);
    case 't':
        return lept_parse_literal(c, v, "true", LEPT_TRUE);
    case 'f':
        return lept_parse_literal(c, v, "false", LEPT_FALSE);
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
