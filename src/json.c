#include <assert.h>
#include <stdlib.h>
#include "json.h"

#define EXPECT(c, ch)   do {assert(*c->json == (ch)); c->json++;} while(0)
#define ISDIGIT(ch)     ((ch) >= '0' && (ch) <= '9')
#define ISDIGIT1T09(ch) ((ch) >= '1' && (ch) <= '9')
#define PUTC(c, ch)     do { *(char*)json_context_push(c, sizeof char) = (ch); } while(0)

typedef struct 
{
    const char *json;
}json_context;

static void json_parse_whitespace(json_context* c)
{
    const char* p = c->json;
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') ++p;
    c->json = p;
}

static int json_parse_number(json_context* c, json_value* v) {
    char* end;
    /* \TODO validate number */
    v->n = strtod(c->json, &end);
    if (c->json == end)
        return JSON_PARSE_INVALID_VALUE;
    c->json = end;
    v->type = JSON_NUMBER;
    return JSON_PARSE_OK;
}

static int json_parse_literal(json_context* c, json_value* v, const char* literal, json_type type)
{
    size_t i;
    EXPECT(c, literal[0]);
    for (i = 0; literal[i + 1]; ++i)
    {
        if (c->json[i] != literal[i + 1]) return JSON_PARSE_INVALID_VALUE;
    }
    c->json += i;
    v->type = type;
    return JSON_PARSE_OK;
}

static int json_parse_value(json_context* c, json_value* v)
{
    switch (*c->json)
    {
        case 't': return json_parse_literal(c, v, "true", JSON_TRUE);
        case 'f': return json_parse_literal(c, v, "false", JSON_FALSE);
        case 'n': return json_parse_literal(c, v, "null", JSON_NULL);
        default: return json_parse_number(c, v);
        case '\0': return JSON_PARSE_EXPECT_VALUE;
    }
}

int json_parse(json_value* v, const char* json)
{
    json_context c;
    assert(v != NULL);
    c.json = json;
    v->type = JSON_NULL;
    json_parse_whitespace(&c);

    int ret;
    if ((ret = json_parse_value(&c, v)) == JSON_PARSE_OK)
    {
        json_parse_whitespace(&c);
        if (*c.json != '\0') ret = JSON_PARSE_ROOT_NOT_SINGULAR;
    }
    return ret;
}

json_type json_get_type(const json_value* v)
{
    assert(v != NULL);
    return v->type;
}