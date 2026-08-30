#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define TEXT_WIDTH 50

typedef struct {
   char  *content;
   size_t position;
   size_t size;
} String;

void String_init(String *s)
{
   s->size       = 1;
   s->position   = 0;
   s->content    = malloc(sizeof(char) * s->size);
   s->content[0] = '\0';
}

#define String_append(string, content)            \
   _Generic((content),                            \
      char *: _String_append_str,                 \
      char: _String_append_char)(string, content)

void _String_append_str(String *s, char *c)
{
   for ( int i = 0; c[i] != '\0'; i++ ) {
      if ( s->position >= s->size - 1 ) {
         s->size   *= 2;
         s->content = realloc(s->content, sizeof(char) * s->size);
      }

      s->content[s->position] = c[i];
      s->position++;
      s->content[s->position] = '\0';
   }
}

void _String_append_char(String *s, char c)
{
   char *temp = malloc(sizeof(char) * 2); // one for our char + one null byte
   temp[0]    = c;
   temp[1]    = '\0';
   _String_append_str(s, temp);
   free(temp);
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

void String_display(String *s)
{
   printf("%s", s->content);
}

void String_format(String *s)
{
   size_t line_pos = 0;

   const char *allowed_around =
      "\n ;:,-+*/={}()[]<>&|"; // if char around is one of thoses you can remove
                               // the space

   // we dont check the first position. we expect
   // the program to start with no space
   int  i, position = 1;
   bool skip_newline = true;
   if ( s->content[0] == '#' ) {
      skip_newline = false;
   }
   for ( i = 1; i < s->position - 1; i++ ) {
      switch ( s->content[i] ) {
         case ' ':
            if ( char_in_str(s->content[i - 1], allowed_around) ||
                 char_in_str(s->content[i + 1], allowed_around) ) {
               continue;
            }
            if ( s->content[i - 2] == '\\' && s->content[i - 1] == 'n' ) {
               continue;
            }
            break;
         case '#':
            if ( s->content[i - 1] == '\n' ) {
               skip_newline = false;
            }
            break;
         case '\\':
            if ( s->content[i + 1] == '\n' ) {
               i++;
               continue;
            }
            break;
         case '\n':
            if ( line_pos > TEXT_WIDTH ) {
               skip_newline = false;
            }

            bool should_newline_prev_line =
               (s->content[i + 1] == '#') && (line_pos > 1);
            if ( skip_newline && !should_newline_prev_line ) {
               continue;
            } else {
               line_pos     = 0;
               skip_newline = true;
            }
            break;
      }

      s->content[position] = s->content[i];
      position++;
      line_pos++;
   }

   s->content[position] = s->content[i];
   position++;
   s->position             = position;
   s->content[s->position] = '\0';
}

void get_self(String *s)
{
   const char *src = "?";

   for ( int i = 0; src[i] != '\0'; i++ ) {
      if ( src[i] == 63 ) {
         for ( int j = 0; src[j] != '\0'; j++ ) {
            switch ( src[j] ) {
               case '\\':
               case '"':
                  String_append(s, "\\");
                  goto DEFAULT;
               case '\n':
                  String_append(s, "\\n");
                  break;
               DEFAULT:
               default:
                  String_append(s, src[j]);
                  break;
            }
         }
      } else {
         String_append(s, src[i]);
      }
   }
}

int main(int argc, char *argv[])
{
   String self;
   String_init(&self);

   get_self(&self);
   String_format(&self);
   String_display(&self);

   return 0;
}
