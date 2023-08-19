#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

enum {
	FAILURE = -1,
	SUCCESS,
};

typedef enum {
	META_CMD_SUCCESS,
	META_CMD_UNRECOGNISED
} meta_cmd_result_t;

typedef enum {
	PREPARE_SUCCESS,
	PREPARE_STMT_UNRECOGNISED
} prepare_result_t;

typedef enum {
	STATEMENT_INSERT,
	STATEMENT_SELECT
} statement_type_t;

typedef struct {
	statement_type_t type;
} statement_t;

typedef struct {
	char *buf;
	size_t buflen;
	ssize_t inlen;
} input_buf_t;

input_buf_t *input_buf_new(void);
int read(input_buf_t *input_buf);
void input_buf_close(input_buf_t *input_buf);
meta_cmd_result_t process_meta_cmd(input_buf_t *input_buf);
prepare_result_t statement_prepare(const input_buf_t *input_buf, statement_t *stmt);
void statement_execute(const statement_t *stmt);
void prompt(void);
void help(void);

int main(void)
{
	char ver[] = "0.1.0";
	char head[] = "SQLeavy version %s\nConnected to a transient in-memory database.\nEnter \".help\" for usage hints.\nEnter \".exit\" to exit out of this program.\n";

	fprintf(stdout, head, ver);

	input_buf_t *input_buf = input_buf_new();

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
					fprintf(stderr, "Unrecognised command: %s\n", input_buf->buf);
					continue;
				} break;
			}
		}

		statement_t statement;
		switch (statement_prepare(input_buf, &statement)) {
			case PREPARE_SUCCESS: {
				break;
			} break;
			case PREPARE_STMT_UNRECOGNISED: {
				fprintf(stderr, "Unrecognised keyword at beginning of: %s\n", input_buf->buf);
				continue;
			} break;
		}

		statement_execute(&statement);
		fputs("Statement executed.\n", stdout);
	}

	return SUCCESS;
}

input_buf_t *input_buf_new(void)
{
	input_buf_t *input_buf = (input_buf_t *)malloc(sizeof(input_buf_t));
	if (!input_buf) {
		fprintf(stderr, "Error allocating memory.");
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

prepare_result_t statement_prepare(const input_buf_t *input_buf, statement_t *stmt)
{
	if (strncmp(input_buf->buf, "insert", 6) == 0) {
		stmt->type = STATEMENT_INSERT;
		return PREPARE_SUCCESS;
	}

	if (strcmp(input_buf->buf, "select") == 0) {
		stmt->type = STATEMENT_SELECT;
		return PREPARE_SUCCESS;
	}

	return PREPARE_STMT_UNRECOGNISED;
}

void statement_execute(const statement_t *stmt)
{
	switch (stmt->type) {
		case STATEMENT_INSERT: {
			fprintf(stdout, "Not implemented: insert.\n");
		} break;
		case STATEMENT_SELECT: {
			fprintf(stdout, "Not implemented: select.\n");
		} break;
	}
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
