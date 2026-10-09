#include "studio.h"
#include <cwctype>
#include <cstring>

static const wchar_t *kw_keywords[] = {
    L"addhandler",L"and",L"andalso",L"as",L"async",L"await",
    L"call",L"case",L"catch",L"class",L"const",L"continue",
    L"delegate",L"dim",L"do",L"each",L"else",L"elseif",L"end",
    L"enum",L"event",L"exit",L"finally",L"for",L"friend",
    L"function",L"get",L"handles",L"if",L"implements",L"imports",
    L"in",L"inherits",L"interface",L"is",L"isnot",L"loop",
    L"me",L"mod",L"module",L"mustinherit",L"mustoverride",
    L"mybase",L"myclass",L"namespace",L"new",L"next",L"not",
    L"nothing",L"of",L"or",L"orelse",L"optional",L"overridable",
    L"overrides",L"partial",L"private",L"property",L"protected",
    L"public",L"raiseevent",L"readonly",L"return",L"select",
    L"set",L"shared",L"static",L"step",L"structure",L"sub",
    L"then",L"throw",L"to",L"true",L"false",L"try",L"using",
    L"while",L"with",L"writeonly",L"xor",L"byref",L"byval",
    nullptr
};

static const wchar_t *kw_types[] = {
    L"boolean",L"byte",L"char",L"decimal",L"double",L"integer",
    L"long",L"object",L"sbyte",L"short",L"single",L"string",
    L"uinteger",L"ulong",L"ushort",L"void",
    nullptr
};

static std::wstring to_lower_w(const wchar_t *s, int len) {
    std::wstring r(s, len);
    for (auto &c : r) c = towlower(c);
    return r;
}

static bool kw_match(const wchar_t **table, const wchar_t *word, int len) {
    auto lower = to_lower_w(word, len);
    for (int i = 0; table[i]; i++) {
        if ((int)wcslen(table[i]) == len && wcscmp(table[i], lower.c_str()) == 0)
            return true;
    }
    return false;
}

static bool is_ident_start(wchar_t c) { return iswalpha(c) || c == L'_'; }
static bool is_ident_cont(wchar_t c)  { return iswalnum(c) || c == L'_'; }
static bool is_digit(wchar_t c)       { return c >= L'0' && c <= L'9'; }
static bool is_hex(wchar_t c) {
    return (c >= L'0' && c <= L'9') || (c >= L'a' && c <= L'f') || (c >= L'A' && c <= L'F');
}

std::vector<TokenSpan> HlTokenize(const wchar_t *text, int length) {
    std::vector<TokenSpan> spans;

    auto push = [&](int start, int len, NlSyntaxToken tt) {
        spans.emplace_back(start, len, tt);
    };

    int i = 0;
    while (i < length) {
        wchar_t c = text[i];

        if (iswspace(c)) { i++; continue; }

        /* Line comment ' */
        if (c == L'\'') {
            int start = i;
            while (i < length && text[i] != L'\n') i++;
            push(start, i - start, TT_COMMENT);
            continue;
        }

        /* Rem comment (whole line) */
        if ((c == L'R' || c == L'r') && i+2 < length &&
            towlower(text[i+1]) == L'e' && towlower(text[i+2]) == L'm' &&
            (i+3 >= length || !is_ident_cont(text[i+3]))) {
            int start = i;
            while (i < length && text[i] != L'\n') i++;
            push(start, i - start, TT_COMMENT);
            continue;
        }

        /* String literal */
        if (c == L'"') {
            int start = i++;
            while (i < length && text[i] != L'\n') {
                if (text[i] == L'"') {
                    if (i+1 < length && text[i+1] == L'"') { i += 2; continue; }
                    i++; break;
                }
                i++;
            }
            if (i < length && (text[i] == L'c' || text[i] == L'C')) i++;
            push(start, i - start, TT_STRING);
            continue;
        }

        /* Hex literal &H */
        if (c == L'&' && i+1 < length && (text[i+1] == L'H' || text[i+1] == L'h')) {
            int start = i; i += 2;
            while (i < length && is_hex(text[i])) i++;
            push(start, i - start, TT_NUMBER);
            continue;
        }

        /* Number */
        if (is_digit(c) || (c == L'.' && i+1 < length && is_digit(text[i+1]))) {
            int start = i;
            while (i < length && is_digit(text[i])) i++;
            if (i < length && text[i] == L'.') {
                i++;
                while (i < length && is_digit(text[i])) i++;
            }
            if (i < length && (text[i] == L'E' || text[i] == L'e')) {
                i++;
                if (i < length && (text[i] == L'+' || text[i] == L'-')) i++;
                while (i < length && is_digit(text[i])) i++;
            }
            if (i < length) {
                wchar_t s = towlower(text[i]);
                if (s == L'l' || s == L'f' || s == L'd' || s == L'i' || s == L's') i++;
            }
            push(start, i - start, TT_NUMBER);
            continue;
        }

        /* Identifier / keyword */
        if (is_ident_start(c)) {
            int start = i;
            while (i < length && is_ident_cont(text[i])) i++;
            int len = i - start;
            NlSyntaxToken tt;
            if (kw_match(kw_keywords, text + start, len))      tt = TT_KEYWORD;
            else if (kw_match(kw_types, text + start, len))    tt = TT_TYPE_KW;
            else                                                tt = TT_IDENT;
            push(start, len, tt);
            continue;
        }

        /* Operators / punctuation */
        if (c == L'+' || c == L'-' || c == L'*' || c == L'/' ||
            c == L'\\' || c == L'^' || c == L'=' || c == L'<' ||
            c == L'>' || c == L'&' || c == L'.' || c == L',' ||
            c == L'(' || c == L')' || c == L'!' || c == L':' ||
            c == L'@' || c == L'?' || c == L'#') {
            push(i, 1, TT_OPERATOR);
            i++; continue;
        }

        push(i, 1, TT_DEFAULT);
        i++;
    }
    return spans;
}

COLORREF HlTokenColor(NlSyntaxToken tt) {
    switch (tt) {
        case TT_KEYWORD:  return g_theme.fg_keyword;
        case TT_TYPE_KW:  return g_theme.fg_type;
        case TT_STRING:   return g_theme.fg_string;
        case TT_COMMENT:  return g_theme.fg_comment;
        case TT_NUMBER:   return g_theme.fg_number;
        case TT_OPERATOR: return g_theme.fg_operator;
        case TT_ERROR:    return g_theme.fg_error;
        default:          return g_theme.fg_default;
    }
}
