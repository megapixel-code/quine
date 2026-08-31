#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#pragma once

#define String_append(string, content)            \
   _Generic((content),                            \
      char *: _String_append_str,                 \
      char: _String_append_char)(string, content)

#define String_insert(string, position, content)            \
   _Generic((content),                                      \
      char *: _String_insert_str,                           \
      char: _String_insert_char)(string, position, content)

#define same_str(buffer, pattern, ...)                         \
   _same_str(buffer, pattern, (_same_str_opts){ __VA_ARGS__ })

const char *src = "?";

typedef struct {
   char  *content;
   size_t last_elmt;
   size_t size;
} String;

void String_init(String *s)
{
   s->size                  = 1;
   s->last_elmt             = 0;
   s->content               = (char *)malloc(sizeof(char) * s->size);
   s->content[s->last_elmt] = '\0';
}

void String_free(String *s)
{
   free(s->content);
}

void String_display(String *s)
{
   printf("%s", s->content);
}

void String_ensure_size(String *s, size_t size)
{
   while ( s->size < size + 1 ) {
      s->size   *= 2;
      s->content = (char *)realloc(s->content, sizeof(char) * s->size);
   }
}

void _String_append_char(String *s, char c)
{
   String_ensure_size(s, s->last_elmt + 1);

   s->content[s->last_elmt] = c;
   s->last_elmt++;
   s->content[s->last_elmt] = '\0';
}

void _String_append_str(String *s, char *c)
{
   for ( int i = 0; c[i] != '\0'; i++ ) {
      _String_append_char(s, c[i]);
   }
}

void _String_insert_char(String *s, int pos, char c)
{
   s->last_elmt++;
   String_ensure_size(s, s->last_elmt);

   for ( int i = s->last_elmt; i > pos; i-- ) {
      s->content[i] = s->content[i - 1];
   }
   s->content[pos] = c;
}

void _String_insert_str(String *s, int pos, char *c)
{
   for ( int i = 0; c[i] != '\0'; i++ ) {
      _String_insert_char(s, pos + i, c[i]);
   }
}

typedef struct {
   bool strict; // check if the strings end in the same place
} _same_str_opts;

bool _same_str(char *buffer, char *pattern, _same_str_opts opts)
{
   int i;
   for ( i = 0; pattern[i] != '\0'; i++ ) {
      if ( pattern[i] != buffer[i] ) {
         return false;
      }
   }
   if ( opts.strict == true ) {
      if ( pattern[i] != buffer[i] ) {
         return false;
      }
   }
   return true;
}

bool char_in_str(char c, const char *s)
{
   for ( int i = 0; s[i] != '\0'; i++ ) {
      if ( s[i] == c ) {
         return true;
      }
   }
   return false;
}
