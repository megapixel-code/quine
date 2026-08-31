#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define TEXT_WIDTH 60

const char *src = "?";
#include "lib.h"

void String_add_newlines(String *s)
{
   const char *add_newlines_around = " ;:,{}()[]";

   bool in_string  = false;
   bool in_tag     = s->content[0] == '#';
   int  line_count = 0;
   for ( int i = 1; s->content[i] != '\0'; i++ ) {
      line_count++;

      switch ( s->content[i] ) {
         case '#':
            if ( s->content[i - 1] == '\n' ) {
               in_tag = true;
            }
            break;
         case '\n':
            line_count = 0;
            in_tag     = false;
            continue;
         case '"':
            if ( s->content[i - 1] == '\\' || s->content[i - 1] == '\'' ) {
               break;
            }
            in_string = !in_string;
            continue; // we do not want to add newline
      }

      if ( (line_count < TEXT_WIDTH) || in_tag ) {
         continue;
      }

      if ( in_string ) {
         if ( s->content[i - 1] == '\\' ) {
            continue;
         }
         line_count = 1;
         String_insert(s, i, "\"\n\"");
         i += 3;
      } else if ( char_in_str(s->content[i], add_newlines_around) ||
                  char_in_str(s->content[i - 1], add_newlines_around) ) {
         line_count = 0;
         String_insert(s, i, (char)'\n');
         i++;
      }
   }
}

void get_self(String *s)
{
   String temp;

   String_init(&temp);
   String_append(&temp, (char *)src);

   for ( int i = 0; temp.content[i] != '\0'; i++ ) {
      if ( temp.content[i] != 63 ) {
         String_append(s, temp.content[i]);
         continue;
      }

      for ( int j = 0; temp.content[j] != '\0'; j++ ) {
         switch ( temp.content[j] ) {
            case '\n':
               String_append(s, "\\n");
               break;
            case '\\':
            case '"':
               String_append(s, (char)'\\');
            default:
               String_append(s, temp.content[j]);
               break;
         }
      }
   }

   String_free(&temp);
}

int main(int argc, char *argv[])
{
   String self;

   String_init(&self);
   get_self(&self);
   String_add_newlines(&self);

   String_display(&self);

   return 0;
}
