
//
// Copyright (c) 2013-2015, John Mettraux, jmettraux+flon@gmail.com
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

// https://github.com/flon-io/shervin

#define _POSIX_C_SOURCE 200809L

#include <stdlib.h>
#include <string.h>
//#include <netinet/in.h>

#include "flutim.h"
#include "gajeta.h"
#include "shv_protected.h"


//
// misc

char fshv_method_to_char(char *s)
{
  if (strncmp(s, "GET", 3) == 0) return 'g';
  if (strncmp(s, "PUT", 3) == 0) return 'u';
  if (strncmp(s, "POST", 4) == 0) return 'p';
  if (strncmp(s, "HEAD", 4) == 0) return 'h';
  if (strncmp(s, "TRACE", 5) == 0) return 't';
  if (strncmp(s, "DELETE", 6) == 0) return 'd';
  if (strncmp(s, "OPTIONS", 7) == 0) return 'o';
  if (strncmp(s, "CONNECT", 7) == 0) return 'c';
  return '?';
}

char *fshv_char_to_method(char c)
{
  if (c == 'g') return "GET";
  if (c == 'u') return "PUT";
  if (c == 'p') return "POST";
  if (c == 'h') return "HEAD";
  if (c == 't') return "TRACE";
  if (c == 'd') return "DELETE";
  if (c == 'o') return "OPTIONS";
  if (c == 'c') return "CONNECT";
  return "???";
}

