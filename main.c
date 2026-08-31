#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define TEXT_WIDTH 60

#define String_append(string, content)            \
   _Generic((content),                            \
      char *: _String_append_str,                 \
      char: _String_append_char)(string, content)

#define String_insert(string, position, content)            \
   _Generic((content),                                      \
      char *: _String_insert_str,                           \
      char: _String_insert_char)(string, position, content)

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
   s->content               = malloc(sizeof(char) * s->size);
   s->content[s->last_elmt] = '\0';
}

void String_free(String *s)
{
   free(s->content);
}

void String_ensure_size(String *s, size_t size)
{
   while ( s->size < size + 1 ) {
      s->size   *= 2;
      s->content = realloc(s->content, sizeof(char) * s->size);
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

void String_remove_spaces(String *s)
{
   const char *remove_space_around =
      "\n ;:,-+*/={}()[]<>&|"; // if char around is one of thoses you can remove
                               // the space

   // we dont check the first position. we expect
   // the program to start with no space
   int  i, position = 1;
   bool in_quotes    = false;
   bool skip_newline = !(s->content[0] == '#');
   for ( i = 1; i < s->last_elmt - 1; i++ ) {
      switch ( s->content[i] ) {
         case '"':
            if ( s->content[i - 1] == '\\' || s->content[i - 1] == '\'' ) {
               break;
            }
            in_quotes = !in_quotes;
            break;
         case ' ':
            if ( (char_in_str(s->content[i - 1], remove_space_around) ||
                  char_in_str(s->content[i + 1], remove_space_around)) &&
                 !in_quotes ) {
               continue;
            }
            break;
         case '#':
            if ( s->content[i - 1] == '\n' ) {
               skip_newline = false;
            }
            break;
         case '\n':
            if ( skip_newline ) {
               continue;
            } else {
               skip_newline = true;
            }
            break;
      }

      s->content[position] = s->content[i];
      position++;
   }

   s->content[position] = s->content[i];
   position++;
   s->last_elmt             = position;
   s->content[s->last_elmt] = '\0';
}

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
   String_remove_spaces(&temp);

   for ( int i = 0; temp.content[i] != '\0'; i++ ) {
      if ( temp.content[i] != 63 ) {
         String_append(s, temp.content[i]);
         continue;
      }

      for ( int j = 0; temp.content[j] != '\0'; j++ ) {
         switch ( temp.content[j] ) {
            case '\\':
            case '"':
               String_append(s, (char)'\\');
               goto DEFAULT;
            case '\n':
               String_append(s, "\\n");
               break;
            DEFAULT:
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
