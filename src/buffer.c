#include "buffer.h"

const uint32_t ID_SIZE = size_of_attribute(Row, id);
const uint32_t USERNAME_SIZE = size_of_attribute(Row, username);
const uint32_t EMAIL_SIZE = size_of_attribute(Row, email);
const uint32_t ID_OFFSET = 0;
const uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
const uint32_t EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE;
const uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;

const uint32_t PAGE_SIZE = 4096;
const uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
const uint32_t TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES;

// here we simply initialize and prepare the buffer for good using
// we don't want that our members of the sturcture have some random value (like 19234232 )
InputBuffer* new_input_buffer() {
  InputBuffer* input_buffer = (InputBuffer*)malloc(sizeof(InputBuffer));
  input_buffer->buffer = NULL;
  input_buffer->buffer_length = 0;
  input_buffer->input_length = 0;

  return input_buffer;
}

// here we use a structure and a function getline (stdio, no way man) to get(read) the data 
// from the user input
void read_input(InputBuffer* input_buffer) {
  ssize_t bytes_read = getline(&(input_buffer->buffer), &(input_buffer->buffer_length), stdin);

  if(bytes_read <= 0){
    printf("Error reading input_buffer\n");
    exit(EXIT_FAILURE);
  }

  // get rid of the '\n' (enter) in the buffer
  input_buffer->input_length = bytes_read - 1;
  input_buffer->buffer[bytes_read - 1] = 0;
}

// clean memory after using 
// we don't need memory leak
void close_input_buffer(InputBuffer* input_buffer){
  free(input_buffer->buffer);
  free(input_buffer);
}

// we check if the user enter the right(valid) statement 
// by deviding the entire sentence on words (by spaces in simple words)
PrepareResult prepare_insert(InputBuffer* input_buffer, Statement* statement) {
  // set statement for the insert
  statement->type = STATEMENT_INSERT;  // as I understand we need it because later in the main 
                                       // function we use statement for the right execution (execute_statement(statement, table)) 
  
  // using function strtok we devide sentence on the word by spaces 
  char* keyword = strtok(input_buffer->buffer, " ");
  char* id_string = strtok(NULL, " ");
  char* username = strtok(NULL, " ");
  char* email = strtok(NULL, " ");
  
  // because I use strong checking, where even unused variables or function give an error, I need to do it for avoiding error
  // I know it would be better if i just disable that that option, but it would be better for me if it will be turn on
  (void)keyword;
  
  // simple checking if the user enter sentence valid, I mean all option 
  if (id_string == NULL || username == NULL || email == NULL) {
    return PREPARE_SYNTAX_ERROR;
  }

  int id = atoi(id_string);
  // Id cannot be negative
  if (id < 0) {
    return PREPARE_NEGATIVE_ID;
  }
  // and next two checking the bonds of the input that user enter
  if (strlen(username) > COLUMN_USERNAME_SIZE) {
    return PREPARE_STRING_TOO_LONG;
  }
  if (strlen(email) > COLUMN_EMAIL_SIZE) {
    return PREPARE_STRING_TOO_LONG;
  }

  // if all the value okay, we set (copy) the value to the structure 
  statement->row_to_insert.id = id;
  strcpy(statement->row_to_insert.username, username);
  strcpy(statement->row_to_insert.email, email);

  return PREPARE_SUCCESS;
}

// what we do here is "prepare", set the right value for the command that user enter
// that is, insert -> initialize everything and check if that valid data 
// select set the statement type for the select 
PrepareResult prepare_statement(InputBuffer* input_buffer,
                                 Statement* statement) {
  if (strncmp(input_buffer->buffer, "insert", 6) == 0) {
    return prepare_insert(input_buffer, statement);
  }
  if (strncmp(input_buffer->buffer, "select", 6) == 0) {
    statement->type = STATEMENT_SELECT;
    return PREPARE_SUCCESS;
  }

  return PREPARE_UNRECOGNIZED_STATEMENT;
}

// okay, I call it raw writing
// yes, as I understand it take, the destionation (it's a persistant memory, in our case file)
// and for right structure it's write every data in right position using offsets
// let's explain by example if our id is 8 bytes, the offset for username will be 8, and the ID_SIZE will be 8 
// but offset for the ID will be 0 because it is the beggining
// so we just copy the memory from the structure that is in RAM, into the file, in raw format
// that is, when we open that file it all will have unrecognized symbols
void serialize_row(Row* source, void* destination) {
  memcpy(destination + ID_OFFSET, &(source->id), ID_SIZE);
  memcpy(destination + USERNAME_OFFSET, &(source->username), USERNAME_SIZE);
  memcpy(destination + EMAIL_OFFSET, &(source->email), EMAIL_SIZE);
}

// here the raw reading happening 
// we take the data from the file(raw data), and set it into the structure
void deserialize_row(void* source, Row* destination) {
  memcpy(&(destination->id), source + ID_OFFSET, ID_SIZE);
  memcpy(&(destination->username), source + USERNAME_OFFSET, USERNAME_SIZE);
  memcpy(&(destination->email), source + EMAIL_OFFSET, EMAIL_SIZE);
}

// so this function open the file, check this size
// and prepare the structure for the work with the data file
Pager* pager_open(const char* filename) {
  // open the file
  int fd = open(filename, 
                O_RDWR |     // read/write mode
                  O_CREAT,  // create file if does not exist 
                S_IWUSR |    // user write permission
                  S_IRUSR    // user read permission
                );

  if (fd == -1 ) {
    printf("Unable to open file\n");
    exit(EXIT_FAILURE);
  }

  // get the lenght of the file
  off_t file_length = lseek(fd, 0, SEEK_END);

  // create structure Pager and set file descriptor with actual length of the file 
  Pager* pager = (Pager*)malloc(sizeof(Pager));
  pager->file_descriptor = fd;
  pager->file_length = file_length;
  
  // setting all the data inside "pages" to zero
  memset(pager->pages, 0, sizeof(pager->pages));

  return pager;
}

// here we initialize full table 
// and set value for it
Table* db_open(const char* filename) {
  
  // first of all we open the file and initalize the Pager structure
  Pager* pager  = pager_open(filename);
  // we see, how much rows we get, as I understand we need it to see 
  // how many rows is already there, i mean wrote 
  uint32_t num_rows = pager->file_length / ROW_SIZE;
  
  // initialize table and set the value for it
  Table* table = (Table*)malloc(sizeof(Table));
  table->pager = pager;
  table->num_rows = num_rows;
  
  return table;
}

// get_page
// so as I understand it simply gets the data from "pages"(sturcture pager)
// and if there is no data in structure, it reads from file 
void* get_page(Pager* pager, uint32_t page_num) {
  // simple border check if there is too big page_num
  if (page_num > TABLE_MAX_PAGES) {
    printf("Tried to fetch page number out of bounds. %d > %d\n", page_num, TABLE_MAX_PAGES);
    exit(EXIT_FAILURE);
  }
  
  // is there is no such pages in the structure
  // then it should check for that page in the file
  if (pager->pages[page_num] == NULL) {
    // Cache miss. Allocate memory and load from file.
    void* page = malloc(PAGE_SIZE);
    uint32_t num_pages = pager->file_length / PAGE_SIZE; 

    // We might save a partial page at the end of the file
    if (pager->file_length % PAGE_SIZE) {
      num_pages += 1;
    }
    
    if (page_num <= num_pages) {
      lseek(pager->file_descriptor, page_num * PAGE_SIZE, SEEK_SET);
      ssize_t bytes_read = read(pager->file_descriptor, page, PAGE_SIZE);
      if (bytes_read == -1) {
        printf("Error reading file: %d\n", errno);
        exit(EXIT_FAILURE);
      }
    }
  
    pager->pages[page_num] = page;
  }

  return pager->pages[page_num];
}

Cursor* table_start(Table* table) {
  Cursor* cursor = (Cursor*)malloc(sizeof(Cursor));
  cursor->table = table;
  cursor->row_num = 0;
  cursor->end_of_table = (table->num_rows == 0);

  return cursor;
}

Cursor* table_end(Table* table) {
  Cursor* cursor = (Cursor*)malloc(sizeof(Cursor));
  cursor->table = table;
  cursor->row_num = table->num_rows;
  cursor->end_of_table = true;

  return cursor;
}

void cursor_advance(Cursor* cursor) {
  cursor->row_num += 1;
  if (cursor->row_num >= cursor->table->num_rows) {
    cursor->end_of_table = true;
  }
}

void* cursor_value(Cursor* cursor) {
  uint32_t row_num = cursor->row_num;
  uint32_t page_num = row_num / ROWS_PER_PAGE;
  
  void* page = get_page(cursor->table->pager, page_num);

  uint32_t row_offset = row_num % ROWS_PER_PAGE;
  uint32_t byte_offset = row_offset * ROW_SIZE;
  return page + byte_offset;
}

void print_row(Row* row) {
  printf("(%d, %s, %s)\n", row->id, row->username, row->email);
}

ExecuteResult execute_insert(Statement* statement, Table* table) {
  if (table->num_rows >= TABLE_MAX_ROWS) {
    return EXECUTE_TABLE_FULL;
  }

  Row* row_to_insert = &(statement->row_to_insert);
  Cursor* cursor = table_end(table);

  serialize_row(row_to_insert, cursor_value(cursor));
  table->num_rows += 1;
 
  free(cursor);

  return EXECUTE_SUCCESS;
}

ExecuteResult execute_select(Statement* statement, Table* table) {
  (void)statement; // mark as intentionally unused
  Cursor* cursor = table_start(table);

  Row row;
  while(!(cursor->end_of_table)) {
    deserialize_row(cursor_value(cursor), &row);
    print_row(&row);
    cursor_advance(cursor);
  }

  free(cursor);

  return EXECUTE_SUCCESS;
}

ExecuteResult execute_statement(Statement* statement, Table* table) {
  switch (statement->type) {
    case (STATEMENT_INSERT):
      return execute_insert(statement, table);
    case (STATEMENT_SELECT):
      return execute_select(statement, table);
  }
  return EXECUTE_TABLE_FULL;
}

//pager_flush
void pager_flush(Pager* pager, uint32_t page_num, uint32_t size) {
  if (pager->pages[page_num] == NULL) {
    printf("Tried to flush null page\n");
    exit(EXIT_FAILURE);
  }

  off_t offset = lseek(pager->file_descriptor, page_num * PAGE_SIZE, SEEK_SET);

  if (offset == -1) {
    printf("Error seeking: %d\n", errno);
    exit(EXIT_FAILURE);
  }

  ssize_t bytes_written = write(pager->file_descriptor, pager->pages[page_num], size);

  if (bytes_written == -1) {
    printf("Error writing: %d\n", errno);
    exit(EXIT_FAILURE);
  }
}

void db_close(Table* table) {
  Pager* pager = table->pager;
  uint32_t num_full_pages = table->num_rows / ROWS_PER_PAGE;
  
  for (uint32_t i = 0; i < num_full_pages; i++) {
    if (pager->pages[i] == NULL) {
      continue;
    }
    pager_flush(pager, i, PAGE_SIZE);
    free(pager->pages[i]);
    pager->pages[i] = NULL;
  }

  // There may be a partial page to write to the end of the file 
  // This should not be needed after we switch to a B-tree 
  uint32_t num_additional_rows = table->num_rows % ROWS_PER_PAGE;
  if (num_additional_rows > 0) {
    uint32_t page_num = num_full_pages;
    if (pager->pages[page_num] != NULL) {
      pager_flush(pager, page_num, num_additional_rows * ROW_SIZE);
      free(pager->pages[page_num]);
      pager->pages[page_num] = NULL;
    }
  }

  int result = close(pager->file_descriptor);
  if (result == -1) {
    printf("Error closing db file.\n");
    exit(EXIT_FAILURE);
  }
  for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
    void* page = pager->pages[i];
    if (page) {
      free(page);
      pager->pages[i] = NULL;
    }
  }
  free(pager);
  free(table);
}

// simple function that just check if there is some meta command (.***)
MetaCommandResult do_meta_command(InputBuffer* input_buffer, Table* table) {
  if (strcmp(input_buffer->buffer, ".exit") == 0) {
    //close_input_buffer(input_buffer);
    db_close(table);
    exit(EXIT_SUCCESS);
  } else {
    return META_COMMAND_UNRECOGNIZED_COMMAND;
  }
}
