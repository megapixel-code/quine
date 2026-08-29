#include <bits/posix2_lim.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define TEXT_WIDTH 20

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

void String_append(String *s, char *c)
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
         case '\n':
            if ( line_pos > TEXT_WIDTH ) {
               skip_newline = false;
               line_pos     = 0;
            }

            if ( skip_newline ) {
               continue;
            } else {
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

   char *my_char = malloc(sizeof(char) * 2); // one for our char + one null byte
   my_char[1]    = '\0';
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
                  my_char[0] = src[j];
                  String_append(s, my_char);
                  break;
            }
         }
      } else {
         my_char[0] = src[i];
         String_append(s, my_char);
      }
   }
   free(my_char);
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
