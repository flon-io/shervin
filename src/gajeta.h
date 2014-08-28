
//
// Copyright (c) 2013-2014, John Mettraux, jmettraux+flon@gmail.com
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// Made in Japan.
//

// gajeta.h

#ifndef FLON_GAJETA_H
#define FLON_GAJETA_H

#include <stdio.h>
#include <stdarg.h>


#define FGAJ_VERSION "1.0.0"

// 10 't' trace
// 20 'd' debug
// 30 'i' info
// 40 'e' error
// 50 'w' warn

typedef void fgaj_logger(char level, const char *pref, const char *msg);

typedef struct fgaj_conf {
  char color;  // 't', 'T', 'f' for true, True and false respectively
  char level;  // defaults to 30 (info)
  short utc;   // 1 = true, defaults to 0
  char *host;  // defaults to result of gethostname()
  fgaj_logger *logger;
  void *params;
} fgaj_conf;

fgaj_conf *fgaj_conf_get();
void fgaj_conf_reset();

void fgaj_color_stdout_logger(char level, const char *pref, const char *msg);
void fgaj_string_logger(char level, const char *pref, const char *msg);

char *fgaj_level_to_string(char level);
char fgaj_normalize_level(char level);

void fgaj_level_string_free(char *s);

void fgaj_log(char level, const char *pref, const char *format, ...);

void fgaj_t(const char *pref, const char *format, ...);
void fgaj_d(const char *pref, const char *format, ...);
void fgaj_i(const char *pref, const char *format, ...);
void fgaj_e(const char *pref, const char *format, ...);
void fgaj_w(const char *pref, const char *format, ...);

/* Returns a string detailing the current time.
 */
char *fgaj_now();

#endif // FLON_GAJETA_H

