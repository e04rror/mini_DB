#ifndef BUFFER_H
#define BUFFER_H

#include <stdint.h> // for use of uint32, 64 and different data types
#include <stdio.h>  // for use of different input/output functions (stdin, stdout, file etc)
#include <stdlib.h> // malloc, size_t, ssize_t 
#include <string.h> // especially for strncmp and other function that work with array of char (string)

#define COLUMN_USERNAME_SIZE 32                                              // how long (max size) the name should be
#define COLUMN_EMAIL_SIZE 255                                                // how long (max size) the email should be
#define size_of_attribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute) // this is macross: get the size of the element(in bytes)
#define TABLE_MAX_PAGES 100                                                  // setting the border value (in the future it will be changed)

// this structure is needed for good using of value from geline function
// like the value that it is return (data size and etc)
typedef struct {
  char* buffer;
  size_t buffer_length;
  ssize_t input_length;
} InputBuffer;

// for command like exit from program or unrecognized command 
typedef enum {
  META_COMMAND_SUCCESS,
  META_COMMAND_UNRECOGNIZED_COMMAND
} MetaCommandResult;

// for checking if user enter a "valid" statement (command) (good, don't know the command, there is an error in the command)
typedef enum { 
  PREPARE_SUCCESS,
  PREPARE_NEGATIVE_ID,
  PREPARE_STRING_TOO_LONG,
  PREPARE_SYNTAX_ERROR,
  PREPARE_UNRECOGNIZED_STATEMENT
} PrepareResult;

// for checking if operation execute good or not(for checking the status of the operation)
typedef enum { EXECUTE_SUCCESS, EXECUTE_TABLE_FULL } ExecuteResult;

// this enumerartion is needed for selection and checking which operation user want to execute
typedef enum { STATEMENT_INSERT, STATEMENT_SELECT } StatementType;


typedef struct {
  uint32_t id;
  char username[COLUMN_USERNAME_SIZE + 1];
  char email[COLUMN_EMAIL_SIZE + 1];
} Row;

typedef struct {
  StatementType type;
  Row row_to_insert; 
} Statement;


typedef struct {
  uint32_t num_rows;
  void* pages[TABLE_MAX_PAGES];
} Table;

InputBuffer* new_input_buffer();

void read_input(InputBuffer* input_buffer);

void close_input_buffer(InputBuffer* input_buffer);

MetaCommandResult do_meta_command(InputBuffer* input_buffer);

PrepareResult prepare_insert(InputBuffer* input_buffer, Statement* statement);

PrepareResult prepare_statement(InputBuffer* input_buffer, Statement* statement);

void serialize_row(Row* source, void* destination);

void deserialize_row(void* source, Row* destination);

void* row_slot(Table* table, uint32_t row_num);

void print_row(Row* row);

Table* new_table();

void free_table(Table* table);

ExecuteResult execute_insert(Statement* statement, Table* table);

ExecuteResult execute_select(Statement* statement, Table* table);

ExecuteResult execute_statement(Statement* statement, Table* table);

#endif // !BUFFER_H
