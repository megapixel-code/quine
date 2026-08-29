#include <stdbool.h>
#include <stdio.h>

void print_quine_file(char *name, bool in_string)
{
   FILE *f = fopen(name, "r");

   char  *buffer;
   size_t buf_size = 0;

   while ( getline(&buffer, &buf_size, f) != -1 ) {
      bool next_line    = false;
      bool double_space = false;

      for ( size_t i = 0; i < buf_size; i++ ) {
         switch ( buffer[i] ) {
            case '/':
               if ( buffer[i + 1] == '/' ) {
                  next_line = true;
                  break;
               }
               goto DEFAULT;
            case ' ':
               if ( !double_space ) {
                  double_space = true;
                  printf(" ");
               }
               break;
            case '\\':
            case '"':
               if ( in_string ) {
                  printf("\\");
               }
               goto DEFAULT;
            case '?':
               if ( in_string ) {
                  goto DEFAULT;
               } else {
                  print_quine_file(name, true);
               }
               break;
            case '\n':
               if ( in_string ) {
                  printf("\\n");
               } else {
                  printf("\n");
               }
               break;
            case '\0':
               next_line = true;
               break;
            DEFAULT:
            default:
               printf("%c", buffer[i]);
               double_space = false;
               break;
         }

         if ( next_line ) {
            break;
         }
      }
   }
   fclose(f);
}

int main(int argc, char *argv[])
{
   if ( argc != 2 ) {
      fprintf(stderr, "Please provide only 1 arg with the file path");
      return 1;
   }

   print_quine_file(argv[1], false);
}
