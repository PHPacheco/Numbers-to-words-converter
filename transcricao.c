#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>
#include <windows.h>
#include <locale.h>

static const wchar_t* units[] = {
    L"", L"one", L"two", L"three", L"four", L"five", L"six", L"seven", L"eight", L"nine",
    L"ten", L"eleven", L"twelve", L"thirteen", L"fourteen", L"fifteen", L"sixteen", L"seventeen", L"eighteen", L"nineteen"
};

static const wchar_t* tens[] = {
    L"", L"", L"twenty", L"thirty", L"forty", L"fifty", L"sixty", L"seventy", L"eighty", L"ninety"
};

static const wchar_t* hundreds[] = {
    L"", L"one hundred", L"two hundred", L"three hundred", L"four hundred", L"five hundred",
    L"six hundred", L"seven hundred", L"eight hundred", L"nine hundred"
};

void write_hundreds(int number, wchar_t* buffer) {
    if (number == 0) {
        buffer[0] = L'\0';
        return;
    }

    if (number == 100) {
        wcscpy(buffer, L"one hundred");
        return;
    }

    int h = number / 100;
    int r = number % 100;
    int t = r / 10;
    int u = r % 10;

    wchar_t temp[100] = L"";

    if (h > 0) {
        wcscat(temp, hundreds[h]);
    }

    if (r > 0) {
        if (h > 0) wcscat(temp, L" and ");

        if (r < 20) {
            wcscat(temp, units[r]);
        } else {
            wcscat(temp, tens[t]);
            if (u > 0) {
                wcscat(temp, L"-");
                wcscat(temp, units[u]);
            }
        }
    }

    wcscpy(buffer, temp);
}

void number_to_words_with_cents(double value, wchar_t* output) {
    long long int_value = (long long)value;
    int cents = (int) round((value - int_value) * 100);

    if (int_value == 0 && cents == 0) {
        wcscpy(output, L"zero dollars");
        return;
    }

    int billions = int_value / 1000000000;
    int_value %= 1000000000;
    int millions = int_value / 1000000;
    int_value %= 1000000;
    int thousands = int_value / 1000;
    int remainder = int_value % 1000;

    wchar_t parts[4][200] = {L"", L"", L"", L""};
    wchar_t temp[200];

    if (billions > 0) {
        write_hundreds(billions, temp);
        wcscat(parts[0], temp);
        wcscat(parts[0], billions == 1 ? L" billion" : L" billion");
    }

    if (millions > 0) {
        write_hundreds(millions, temp);
        wcscat(parts[1], temp);
        wcscat(parts[1], millions == 1 ? L" million" : L" million");
    }

    if (thousands > 0) {
        if (thousands == 1) {
            wcscpy(parts[2], L"one thousand");
        } else {
            write_hundreds(thousands, temp);
            wcscat(parts[2], temp);
            wcscat(parts[2], L" thousand");
        }
    }

    if (remainder > 0) {
        write_hundreds(remainder, parts[3]);
    }

    wcscpy(output, L"");
    int started = 0;
    for (int i = 0; i < 4; i++) {
        if (wcslen(parts[i]) > 0) {
            if (started)
                wcscat(output, (i == 3) ? L" and " : L", ");
            wcscat(output, parts[i]);
            started = 1;
        }
    }

    if (int_value > 0 || billions > 0 || millions > 0 || thousands > 0 || remainder > 0) {
        wcscat(output, (int_value == 1) ? L" dollar" : L" dollars");
    }

    if (cents > 0) {
        if (started) {
            wcscat(output, L" and ");
        }

        write_hundreds(cents, temp);
        wcscat(output, temp);
        wcscat(output, (cents == 1) ? L" cent" : L" cents");
    }
}

int validate_number_format(const wchar_t* input) {
    int has_comma = 0;

    for (int i = 0; input[i] != L'\0'; i++) {
        if (input[i] == L'.') continue;
        if (input[i] == L',') {
            if (has_comma) return 0;
            has_comma = 1;
        } else if (!iswdigit(input[i])) {
            return 0;
        }
    }

    return 1;
}

int convert_to_double(const wchar_t* input, double* value) {
    wchar_t buffer[100];
    int j = 0;

    for (int i = 0; input[i] != L'\0'; i++) {
        if (input[i] == L'.') continue;
        else if (input[i] == L',') buffer[j++] = L'.';
        else buffer[j++] = input[i];
    }
    buffer[j] = L'\0';

    char* old_locale = setlocale(LC_NUMERIC, NULL);
    old_locale = _strdup(old_locale);

    setlocale(LC_NUMERIC, "C");

    wchar_t* endptr;
    *value = wcstod(buffer, &endptr);

    setlocale(LC_NUMERIC, old_locale);
    free(old_locale);

    return (endptr != buffer && *endptr == L'\0');
}

void add_parentheses(wchar_t* text) {
    size_t length = wcslen(text);
    wchar_t* modified = (wchar_t*)malloc((length + 3) * sizeof(wchar_t));

    if (modified == NULL) {
        wprintf(L"Memory allocation error.\n");
        return;
    }

    modified[0] = L'(';
    wcscpy(&modified[1], text);
    modified[length + 1] = L')';
    modified[length + 2] = L'\0';

    wcscpy(text, modified);
    free(modified);
}

int transcribe_value(const wchar_t* input, wchar_t* output, bool copyToClipboard, bool addParentheses) {
    setlocale(LC_ALL, "");

    if (!validate_number_format(input)) {
        wcscpy(output, L"Invalid input: use only numbers, dot (.) as thousand separator, and comma (,) as decimal separator.");
        return 0;
    }

    double value;
    if (!convert_to_double(input, &value)) {
        wcscpy(output, L"Conversion error: could not convert input to number.");
        return 0;
    }

    if (value < 0 || value >= 1e12) {
        wcscpy(output, L"Value out of supported range: the number must be less than 1 trillion.");
        return 0;
    }

    number_to_words_with_cents(value, output);

    if (addParentheses) {
        add_parentheses(output);
    }

    if (copyToClipboard) {
        size_t len = wcslen(output) + 1;
        HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len * sizeof(wchar_t));
        if (hMem == NULL) return 0;

        wchar_t* wstr = (wchar_t*)GlobalLock(hMem);
        wcscpy(wstr, output);
        GlobalUnlock(hMem);

        if (OpenClipboard(NULL)) {
            EmptyClipboard();
            SetClipboardData(CF_UNICODETEXT, hMem);
            CloseClipboard();
        } else {
            GlobalFree(hMem);
            return 0;
        }
    }

    return 1;
}
