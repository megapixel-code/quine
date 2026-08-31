#include "lib.h"

#include <stdbool.h>
#include <stdio.h>

#define MAIN_PATH "src/main.c"
#define LIB_PATH  "src/lib.h"

void String_remove_char(String *s, int pos)
{
   if ( pos >= s->size ) {
      fprintf(stderr,
              "%s:%s:%d:trying to remove char outside of allocated buffer",
              __FILE__,
              __FUNCTION__,
              __LINE__);
      exit(1);
   }

   for ( int i = pos; i < s->last_elmt; i++ ) {
      s->content[i] = s->content[i + 1];
   }
   if ( s->last_elmt > 0 ) {
      s->last_elmt--;
   }
}

void String_remove_spaces(String *s)
{
   const char *remove_space_around = "\n ;:,-+*/={}()[]<>'&|";

   // we dont check the first position. we expect
   // the program to start with no space
   int  i, position = 1;
   bool in_tag    = s->content[0] == '#';
   bool in_quotes = false;
   for ( i = 1; i < s->last_elmt - 1; i++ ) {
      switch ( s->content[i] ) {
         case ' ':
            if ( in_quotes ) {
               break;
            }
            if ( char_in_str(s->content[i - 1], remove_space_around) ||
                 char_in_str(s->content[i + 1], remove_space_around) ) {
               String_remove_char(s, i);
               i--;
               continue;
            }
            break;
         case '"':
            if ( s->content[i - 1] == '\\' || s->content[i - 1] == '\'' ) {
               break;
            }
            in_quotes = !in_quotes;
            break;
         case '#':
            if ( s->content[i - 1] != '\n' ) {
               break;
            }
            in_tag = true;
            break;
         case '\n':
            if ( in_tag ) {
               in_tag = false;
               while ( s->content[i] == '\n' ) {
                  String_remove_char(s, i);
               }
               String_insert(s, i, (char)'\n');
               break;
            }
            if ( s->content[position + 1] == '#' ) {
               break;
            }

            String_remove_char(s, i);
            i--;
            continue;
         case '\\':
            if ( s->content[i + 1] == '\n' ) {
               String_remove_char(s, i);
               String_remove_char(s, i);
               i--;
               continue;
            }
            break;
         case '/':
            if ( s->content[i + 1] == '/' ) {
               while ( s->content[i] != '\n' ) {
                  String_remove_char(s, i);
               }
               String_remove_char(s, i);
               i--;
               continue;
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

void String_append_file(String *s, char *path, bool is_lib)
{
   FILE *f = fopen(path, "r");

   char  *buffer;
   size_t buf_size = 0;

   // remove everything until "#pragma once"
   if ( is_lib ) {
      do {
         getline(&buffer, &buf_size, f);
      } while ( !same_str(buffer, "#pragma once") );
   }

   while ( getline(&buffer, &buf_size, f) != -1 ) {
      if ( !is_lib && same_str(buffer, "#include \"lib.h\"") ) {
         String_append(s, (char)'\n');
         String_append_file(s, LIB_PATH, true);
         continue;
      }
      String_append(s, buffer);
   }
}

void String_get_quine(String *s)
{
   String_append_file(s, MAIN_PATH, false);
   String_remove_spaces(s);

   String temp;
   String_init(&temp);
   String_append(&temp, s->content);

   int insert_pos = 0;
   while ( s->content[insert_pos] != '?' ) {
      insert_pos++;
   }
   String_remove_char(s, insert_pos);
   for ( int i = 0; temp.content[i] != '\0'; i++ ) {
      switch ( temp.content[i] ) {
         case '\n':
            String_insert(s, insert_pos, "\\n");
            insert_pos += 2;
            break;
         case '\\':
         case '"':
            String_insert(s, insert_pos, (char)'\\');
            insert_pos++;
         default:
            String_insert(s, insert_pos, temp.content[i]);
            insert_pos++;
            break;
      }
   }

   String_free(&temp);
}

int main(int argc, char *argv[])
{
   String quine;
   String_init(&quine);
   String_get_quine(&quine);

   String_display(&quine);
}
