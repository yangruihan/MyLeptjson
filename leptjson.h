#ifndef LEPTJSON_H__
#define LEPTJSON_H__

typedef enum { LEPT_NULL,
               LEPT_FALSE,
               LEPT_TRUE,
               LEPT_NUMBER,
               LEPT_STRING,
               LEPT_ARRAY,
               LEPT_OBJECT } lept_type;

typedef struct
{
    union {
        struct
        {
            char *s;
            size_t len;
        } s;      /* string */
        double n; /* number */
    } u;

    lept_type type;
} lept_value;

typedef struct
{
    const char *json;
    char *stack;
    size_t size, top;
} lept_context;

enum
{
    LEPT_PARSE_OK = 0,
    LEPT_PARSE_EXPECT_VALUE,
    LEPT_PARSE_INVALID_VALUE,
    LEPT_PARSE_ROOT_NOT_SINGULAR,
    LEPT_PARSE_NUMBER_TOO_BIG,
    LEPT_PARSE_MISS_QUOTATION_MARK,
    LEPT_PARSE_INVALID_STRING_ESCAPE,
    LEPT_PARSE_INVALID_STRING_CHAR,
    LEPT_PARSE_INVALID_UNICODE_SURROGATE,
    LEPT_PARSE_INVALID_UNICODE_HEX
};

#define lept_init(v)           \
    do                         \
    {                          \
        (v)->type = LEPT_NULL; \
    } while (0)

#define lept_set_null(v) lept_free(v)

static void lept_parse_whitespace(lept_context *c);
static int check_root_is_not_singular(lept_context *c, lept_value *v);
static int lept_parse_literal(lept_context *c, lept_value *v, char *expect, lept_type type);
static int lept_parse_number(lept_context *c, lept_value *v);
static const char *lept_parse_hex4(const char *p, unsigned *u);
static void lept_encode_utf8(lept_context *c, unsigned u);
static int lept_parse_string(lept_context *c, lept_value *v);
static int lept_parse_value(lept_context *c, lept_value *v);

int lept_parse(lept_value *v, const char *json);

lept_type lept_get_type(const lept_value *v);

int lept_get_boolean(const lept_value *v);
void lept_set_boolean(lept_value *v, int b);

double lept_get_number(const lept_value *v);
void lept_set_number(lept_value *v, double n);

const char *lept_get_string(const lept_value *v);
size_t lept_get_string_length(const lept_value *v);
void lept_set_string(lept_value *v, const char *s, size_t len);

void lept_free(lept_value *v);

#ifndef LEPT_PARSE_STACK_INIT_SIZE
#define LEPT_PARSE_STACK_INIT_SIZE 256
#endif

static void *lept_context_push(lept_context *c, size_t size);
static void *lept_context_pop(lept_context *c, size_t size);

#endif /* LEPTJSON_H__ */
