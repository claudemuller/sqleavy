#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct {
	char *buf;
	size_t buflen;
	ssize_t inlen;
} input_buf_t;

input_buf_t *input_buf_new(void)
{
	input_buf_t *in_buf = (input_buf_t *)malloc(sizeof(input_buf_t));
	if (!in_buf) {
		fprintf(stderr, "Error allocating memory.");
		return NULL;
	}

	in_buf->buf = NULL;
	in_buf->buflen = 0;
	in_buf->inlen = 0;

	return in_buf;
}

void prompt(void)
{
	fprintf(stdout, "db > ");
}

int read(input_buf_t *in_buf)
{
	ssize_t bytes_read = getline(&(in_buf->buf), &(in_buf->buflen), stdin);

	if (bytes_read <= 0) {
		fprintf(stderr, "Error reading input.\n");
		return -1;
	}

	in_buf->inlen = bytes_read - 1; // Trim trailing whitespace
	in_buf->buf[bytes_read - 1] = '\0';

	return 0;
}

void input_buf_close(input_buf_t *in_buf)
{
	if (in_buf->buf != NULL) {
		free(in_buf->buf);
	}
	if (in_buf != NULL) {
		free(in_buf);
	}
}

void help(void)
{
	char help_text[] = ".help - this menu\n.exit - exit this program\n";
	fputs(help_text, stdout);
}

int main(void)
{
	char ver[] = "0.1.0";
	char head[] = "SQLeavy version %s\nConnected to a transient in-memory database.\nEnter \".help\" for usage hints.\nEnter \".exit\" to exit out of this program.\n";

	fprintf(stdout, head, ver);

	input_buf_t *input_buf = input_buf_new();

	while (true) {
		prompt();

		if (read(input_buf) < 0) {
			return -1;
		}

		if (strcmp(input_buf->buf, ".exit") == 0) {
			input_buf_close(input_buf);
			break;
		}
		if (strcmp(input_buf->buf, ".help") == 0) {
			help();
			continue;
		}

		fprintf(stderr, "Unrecognised command: %s.\n", input_buf->buf);
	}

	return 0;
}
