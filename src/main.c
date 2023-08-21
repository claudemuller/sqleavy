#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define COLUMN_USERNAME_MAX 32
#define COLUMN_EMAIL_MAX 255

#define size_of_attr(struct, attr) sizeof(((struct *)0)->attr)

enum {
	FAILURE = -1,
	SUCCESS,
};

typedef enum {
	META_CMD_SUCCESS,
	META_CMD_UNRECOGNISED,
} meta_cmd_result_t;

typedef enum {
	PREPARE_SUCCESS,
	PREPARE_NEGATIVE_ID,
	PREPARE_STRING_TOO_LONG,
	PREPARE_SYNTAX_ERROR,
	PREPARE_STATEMENT_UNRECOGNISED,
} prepare_result_t;

typedef enum {
	EXECUTE_SUCCESS,
	EXECUTE_TABLE_FULL,
} execute_result_t;

typedef enum {
	STATEMENT_INSERT,
	STATEMENT_SELECT,
} statement_type_t;

typedef struct {
	uint32_t id;
	char username[COLUMN_USERNAME_MAX+1];
	char email[COLUMN_EMAIL_MAX+1];
} row_t;

typedef struct {
	statement_type_t type;
	row_t row_to_insert;
} statement_t;

typedef struct {
	char *buf;
	size_t buflen;
	ssize_t inlen;
} input_buf_t;

const uint32_t ID_SIZE = size_of_attr(row_t, id);
const uint32_t USERNAME_SIZE = size_of_attr(row_t, username);
const uint32_t EMAIL_SIZE = size_of_attr(row_t, email);
const uint32_t ID_OFFSET = 0;
const uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
const uint32_t EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE;
const uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;

#define TABLE_MAX_PAGES 100
const uint32_t PAGE_SIZE = 4096;
const uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
const uint32_t TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES;

typedef struct {
	uint32_t num_rows;
	void *pages[TABLE_MAX_PAGES];
} table_t;

table_t *table_new(void);
void table_free(table_t *table);
input_buf_t *input_buf_new(void);
int read(input_buf_t *input_buf);
void input_buf_close(input_buf_t *input_buf);
meta_cmd_result_t process_meta_cmd(input_buf_t *input_buf);
prepare_result_t statement_prepare(input_buf_t *input_buf, statement_t *statement);
execute_result_t statement_execute(statement_t *statement, table_t *table);
execute_result_t statement_insert(statement_t *statement, table_t *table);
execute_result_t statement_select(const statement_t *statement, table_t *table);
void serialise_row(row_t *src, void *dest);
void deserialise_row(void *src, row_t *dest);
void *row_slot(table_t *table, uint32_t row_n);
void print_row(const row_t *row);
void prompt(void);
void help(void);

int main(void)
{
	char ver[] = "0.1.0";
	char head[] = "SQLeavy version %s\nConnected to a transient in-memory database.\nEnter \".help\" for usage hints.\nEnter \".exit\" to exit out of this program.\n";

	fprintf(stdout, head, ver);

	input_buf_t *input_buf = input_buf_new();
	table_t *table = table_new();
	if (!table) {
		return FAILURE;
	}

	while (true) {
		prompt();

		if (read(input_buf) < 0) {
			return FAILURE;
		}

		if (input_buf->buf[0] == '.') {
			switch (process_meta_cmd(input_buf)) {
				case META_CMD_SUCCESS: {
					continue;
				} break;

				case META_CMD_UNRECOGNISED: {
					fprintf(stderr, "Unrecognised command: '%s'\n", input_buf->buf);
					continue;
				} break;
			}
		}

		statement_t statement;
		switch (statement_prepare(input_buf, &statement)) {
			case PREPARE_SUCCESS: {
				break;
			} break;

			case PREPARE_NEGATIVE_ID: {
				fprintf(stderr, "ID must be positive.\n");
				continue;
			} break;

			case PREPARE_STRING_TOO_LONG: {
				fprintf(stderr, "String too long.\n");
				continue;
			} break;

			case PREPARE_SYNTAX_ERROR: {
				fprintf(stderr, "Syntax error in: '%s'\n", input_buf->buf);
				continue;
			} break;

			case PREPARE_STATEMENT_UNRECOGNISED: {
				fprintf(stderr, "Unrecognised keyword at beginning of: '%s'\n", input_buf->buf);
				continue;
			} break;
		}

		switch (statement_execute(&statement, table)) {
			case EXECUTE_SUCCESS: {
				fprintf(stdout, "Statement executed.\n");
	 		} break;

			case EXECUTE_TABLE_FULL: {
				fprintf(stdout, "Error, table is full.\n");
	 		} break;
		}
	}

	return SUCCESS;
}

table_t *table_new(void)
{
	table_t *table = (table_t *)malloc(sizeof(table_t));
	if (!table) {
		fprintf(stdout, "Error allocating memory for table\n");
		return NULL;
	}

	table->num_rows = 0;
	for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
		table->pages[i] = NULL;
	}

	return table;
}

void table_free(table_t *table)
{
	for (int i = 0; table->pages[i]; i++) {
		free(table->pages[i]);
	}
	free(table);
}

input_buf_t *input_buf_new(void)
{
	input_buf_t *input_buf = (input_buf_t *)malloc(sizeof(input_buf_t));
	if (!input_buf) {
		fprintf(stderr, "Error allocating memory for input buffer.");
		return NULL;
	}

	input_buf->buf = NULL;
	input_buf->buflen = 0;
	input_buf->inlen = 0;

	return input_buf;
}

void input_buf_close(input_buf_t *input_buf)
{
	if (input_buf->buf != NULL) {
		free(input_buf->buf);
	}
	if (input_buf != NULL) {
		free(input_buf);
	}
}

int read(input_buf_t *input_buf)
{
	ssize_t bytes_read = getline(&(input_buf->buf), &(input_buf->buflen), stdin);

	if (bytes_read <= 0) {
		fprintf(stderr, "Error reading input.\n");
		return FAILURE;
	}

	input_buf->inlen = bytes_read - 1; // Trim trailing whitespace
	input_buf->buf[bytes_read - 1] = '\0';

	return SUCCESS;
}

meta_cmd_result_t process_meta_cmd(input_buf_t *input_buf)
{
	if (strcmp(input_buf->buf, ".exit") == 0) {
		input_buf_close(input_buf);
		exit(SUCCESS);
	}

	if (strcmp(input_buf->buf, ".help") == 0) {
		help();
		return META_CMD_SUCCESS;
	}

	return META_CMD_UNRECOGNISED;
}

prepare_result_t prepare_insert(input_buf_t *input_buf, statement_t *statement)
{
	statement->type = STATEMENT_INSERT;
	char *keyword = strtok(input_buf->buf, " ");
	char *id_string = strtok(NULL, " ");
	char *username = strtok(NULL, " ");
	char *email = strtok(NULL, " ");

	if (id_string == NULL || username == NULL || email == NULL) {
		return PREPARE_SYNTAX_ERROR;
	}

	int id = atoi(id_string);
	if (id < 0) {
		return PREPARE_NEGATIVE_ID;
	}

	if (strlen(username) > COLUMN_USERNAME_MAX) {
		return PREPARE_STRING_TOO_LONG;
	}

	if (strlen(email) > COLUMN_EMAIL_MAX) {
		return PREPARE_STRING_TOO_LONG;
	}

	statement->row_to_insert.id = id;
	strcpy(statement->row_to_insert.username, username);
	strcpy(statement->row_to_insert.email, email);

	return PREPARE_SUCCESS;
}

prepare_result_t statement_prepare(input_buf_t *input_buf, statement_t *statement)
{
	if (strncmp(input_buf->buf, "insert", 6) == 0) {
		return prepare_insert(input_buf, statement);
	}

	if (strcmp(input_buf->buf, "select") == 0) {
		statement->type = STATEMENT_SELECT;
		return PREPARE_SUCCESS;
	}

	return PREPARE_STATEMENT_UNRECOGNISED;
}

execute_result_t statement_execute(statement_t *statement, table_t *table)
{
	switch (statement->type) {
		case STATEMENT_INSERT: {
			return statement_insert(statement, table);
		} break;
		case STATEMENT_SELECT: {
			return statement_select(statement, table);
		} break;
	}
}

execute_result_t statement_insert(statement_t *statement, table_t *table)
{
	if (table->num_rows >= TABLE_MAX_ROWS) {
		return EXECUTE_TABLE_FULL;
	}

	row_t *row_to_insert = &(statement->row_to_insert);

	serialise_row(row_to_insert, row_slot(table, table->num_rows));
	table->num_rows++;

	return EXECUTE_SUCCESS;
}

execute_result_t statement_select(const statement_t *statement, table_t *table)
{
	row_t row;
	for (uint32_t i = 0; i < table->num_rows; i++) {
		deserialise_row(row_slot(table, i), &row);
		print_row(&row);
	}

	return EXECUTE_SUCCESS;
}

void serialise_row(row_t *src, void *dest)
{
	memcpy(dest + ID_OFFSET, &(src->id), ID_SIZE);
	memcpy(dest + USERNAME_OFFSET, &(src->username), USERNAME_SIZE);
	memcpy(dest + EMAIL_OFFSET, &(src->email), EMAIL_SIZE);
}

void deserialise_row(void *src, row_t *dest)
{
	memcpy(&(dest->id), src + ID_OFFSET, ID_SIZE);
	memcpy(&(dest->username), src + USERNAME_OFFSET, USERNAME_SIZE);
	memcpy(&(dest->email), src + EMAIL_OFFSET, EMAIL_SIZE);
}

void *row_slot(table_t *table, uint32_t row_n)
{
	uint32_t page_n = row_n / ROWS_PER_PAGE;
	void *page = table->pages[page_n];
	if (!page) {
		page = table->pages[page_n] = malloc(PAGE_SIZE);
		if (!page) {
			fprintf(stderr, "Error allocating memory for new page\n");
		}
	}

	uint32_t row_offset = row_n % ROWS_PER_PAGE;
	uint32_t byte_offset = row_offset * ROW_SIZE;

	return page + byte_offset;
}

void print_row(const row_t *row)
{
	fprintf(stdout, "(%d, %s, %s)\n", row->id, row->username, row->email);
}

void prompt(void)
{
	fprintf(stdout, "db > ");
}

void help(void)
{
	char help_text[] = ".help - this menu\n.exit - exit this program\n";
	fputs(help_text, stdout);
}
