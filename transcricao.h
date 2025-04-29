#ifndef TRANSCRIPTION_H
#define TRANSCRIPTION_H

#include <stdbool.h> // For 'bool' type

#ifdef __cplusplus
extern "C" {
#endif

int transcribe_value(const wchar_t* input, wchar_t* output, bool copyToClipboard, bool addParentheses);

#ifdef __cplusplus
}
#endif

#endif
