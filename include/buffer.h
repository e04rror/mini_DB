#ifndef BUFFER_H
#define BUFFER_H

#include <errno.h>
#include <fcntl.h>
#include <stdint.h> // for use of uint32, 64 and different data types
#include <stdio.h>  // for use of different input/output functions (stdin, stdout, file etc)
#include <stdlib.h> // malloc, size_t, ssize_t 
#include <string.h> // especially for strncmp and other function that work with array of char (string)
#include <unistd.h>

#define COLUMN_USERNAME_SIZE 32                                              // how long (max size) the name should be
#define COLUMN_EMAIL_SIZE 70                                             // how long (max size) the email should be
#define size_of_attribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute) // this is macross: get the size of the element(in bytes)
#define TABLE_MAX_PAGES 100                                             // setting the border value (in the future it will be changed)

#define ID_SIZE size_of_attribute(Row, id)
#define USERNAME_SIZE size_of_attribute(Row, username)
#define EMAIL_SIZE size_of_attribute(Row, email)
#define ID_OFFSET 0
#define USERNAME_OFFSET (ID_OFFSET + ID_SIZE)
#define EMAIL_OFFSET (USERNAME_OFFSET + USERNAME_SIZE)
#define ROW_SIZE (ID_SIZE + USERNAME_SIZE + EMAIL_SIZE)
#define PAGE_SIZE 4096




// Node types: Internal nodes store pointers to children, Leaf nodes store actual data
typedef enum { NODE_INTERNAL, NODE_LEAF } NodeType;

uint32_t* leaf_node_num_cells(void* node);

void* leaf_node_cell(void* node, uint32_t cell_num);

uint32_t* leaf_node_key(void* node, uint32_t cell_num);

void* leaf_node_value(void* node, uint32_t cell_num);

void initialize_leaf_node(void* node);

void print_constants();

void print_leaf_node(void* node);

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

// the structure of the full row
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
  int file_descriptor;
  uint32_t file_length;
  uint32_t num_pages;
  void *pages[TABLE_MAX_PAGES];
} Pager;

typedef struct {
  Pager *pager;
  uint32_t root_page_num;
} Table;

typedef struct { 
  Table* table;
  uint32_t page_num;
  uint32_t cell_num;
  bool end_of_table; // Indicates a position one past the last element
} Cursor;

Cursor* table_start(Table* table);

Cursor* table_end(Table* table);

void cursor_advance(Cursor* cursor);

InputBuffer* new_input_buffer();

void read_input(InputBuffer* input_buffer);

void close_input_buffer(InputBuffer* input_buffer);

MetaCommandResult do_meta_command(InputBuffer* input_buffer, Table* table);

PrepareResult prepare_insert(InputBuffer* input_buffer, Statement* statement);

PrepareResult prepare_statement(InputBuffer* input_buffer, Statement* statement);

void serialize_row(Row* source, void* destination);

void deserialize_row(void* source, Row* destination);

void* cursor_value(Cursor* cursor);

void print_row(Row* row);

Pager* pager_open(const char* filename);

Table* db_open(const char* filename);

void free_table(Table* table);

void* get_page(Pager* pager, uint32_t page_num);

void leaf_node_insert(Cursor* cursor, uint32_t key, Row* value);

ExecuteResult execute_insert(Statement* statement, Table* table);

ExecuteResult execute_select(Statement* statement, Table* table);

ExecuteResult execute_statement(Statement* statement, Table* table);

void pager_flush(Pager* pager, uint32_t page_num);

void db_close(Table* table);

#endif // !BUFFER_H
