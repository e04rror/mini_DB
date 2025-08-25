#include "buffer.h"

input_buffer* new_input_buffer() {
  input_buffer* input = (input_buffer*)malloc(sizeof(input_buffer));
  input->buffer = NULL;
  input->buffer_length = 0;
  input->input_length = 0;

  return input;
}

void read_input(input_buffer* input) {
  ssize_t bytes_read = getline(&(input->buffer), &(input->buffer_length), stdin);

  if(bytes_read <= 0){
    printf("Error reading input\n");
    exit(EXIT_FAILURE);
  }

  input->input_length = bytes_read - 1;
  input->buffer[bytes_read - 1] = 0;
}

// clean memory after using 
// we don't need memory leak
void close_input_buffer(input_buffer* input){
  free(input->buffer);
  free(input);
}

meta_command_result do_meta_command(input_buffer* input) {
  if (strcmp(input->buffer, ".exit") == 0) {
    close_input_buffer(input);
    exit(EXIT_SUCCESS);
  } else {
    return META_COMMAND_UNRECOGNIZED_COMMAND;
  }
}

prepare_result prepare_statement(input_buffer* input,
                                 statement* stat) {
  if (strncmp(input->buffer, "insert", 6) == 0) {
    stat->type = STATEMENT_INSERT;
    int args_assigned = sscanf(input->buffer, "insert %d %s %s", %(stat->row_to_insert.id), 
                               stat->row_to_insert.username, stat->row_to_insert.email);
    if(args_assigned < 3) {
      return PREPARE_SYNTAX_ERROR;
    }

    return PREPARE_SUCCESS;
  }
  if (strncmp(input->buffer, "select", 6) == 0) {
    stat->type = STATEMENT_SELECT;
    return PREPARE_SUCCESS;
  }

  return PREPARE_UNRECOGNIZED_STATEMENT;
}

void execute_statement(statement* stat) {
  switch (stat->type) {
    case (STATEMENT_INSERT):
      printf("This is where we would do an insert.\n");
      break;
    case (STATEMENT_SELECT):
      printf("This is where we would do a select.\n");
      break;
  }
}
