#ifndef BUFFER_H
#define BUFFER_H

#include <cstdint>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255

typedef struct {
  char* buffer;
  size_t buffer_length;
  ssize_t input_length;
} input_buffer;

typedef enum {
  META_COMMAND_SUCCESS,
  META_COMMAND_UNRECOGNIZED_COMMAND
} meta_command_result;


typedef enum { PREPARE_SUCCESS, PREPARE_UNRECOGNIZED_STATEMENT } prepare_result;


typedef enum { STATEMENT_INSERT, STATEMENT_SELECT } statement_type;

typedef struct {
  uint32_t id;
  char username[COLUMN_USERNAME_SIZE];
  char email[COLUMN_EMAIL_SIZE];
} row;

typedef struct {
  statement_type type;
  row row_to_insert; 
} statement;


input_buffer* new_input_buffer();

void read_input(input_buffer* input);

void close_input_buffer(input_buffer* input);

meta_command_result do_meta_command(input_buffer* input);

prepare_result prepare_statement(input_buffer* input, statement* stat);

void execute_statement(statement* stat);

#endif // !BUFFER_H
