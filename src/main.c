#include "buffer.h"

void print_prompt() { printf("db > "); }

int main(/*int argc, char* argv[]*/) {
  input_buffer* input = new_input_buffer();
  while (1) {
    print_prompt();
    read_input(input);
    
    if (input->buffer[0] == '.') {
      switch (do_meta_command(input)) {
        case (META_COMMAND_SUCCESS):
          continue;
        case (META_COMMAND_UNRECOGNIZED_COMMAND):
          printf("Unrecognized command '%s'\n", input->buffer);
          continue;
      }
    }

    statement stat;
    switch (prepare_statement(input, &stat)) {
      case (PREPARE_SUCCESS):
        break;
      case (PREPARE_UNRECOGNIZED_STATEMENT):
        printf("Unrecognized keyword at start of '%s'.\n", input->buffer);
        continue;
    }
    execute_statement(&stat);
    printf("Executed.\n");
  }
}
