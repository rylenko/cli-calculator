#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "calculator.h"
#include "error.h"

#define RPN_MAX_LEN 8192
#define INTORPN_STACK_MAX_LEN 1024
#define CALCULATERPN_STACK_MAX_LEN 128

typedef enum {
	CLOSING_BRACKET,
	NUMBER,
	OP,
	OPENING_BRACKET,
	START,
} Element;

static long calculate_rpn(const char *rpn);
static void make_rpn(const char *in, char *rpn);
static unsigned char op_prior(char op);

long
calculate(const char *s)
{
	char rpn[RPN_MAX_LEN];
	make_rpn(s, rpn);
	return calculate_rpn(rpn);
}

/* Calculates reverse polish notation */
static long
calculate_rpn(const char *rpn)
{
	/* Create and initialize a variables */
	long stack[CALCULATERPN_STACK_MAX_LEN],
 		result;
	size_t rpn_i,
   		rpn_len,
   		stack_len = 0;
	char *strtol_end;
	if (!rpn)
		error(22, "`rpn == NULL`.");
	rpn_len = strlen(rpn);

	/* Calculation */
	for (rpn_i = 0; rpn_i < rpn_len; ++rpn_i) {
		if (
			isdigit(rpn[rpn_i])
			|| (
				rpn[rpn_i] == '-'
				&& rpn_i + 1 < rpn_len
				&& isdigit(rpn[rpn_i + 1])
			)
		) {
			stack[stack_len++] = strtol(&rpn[rpn_i], &strtol_end, 10);
			rpn_i = strtol_end - rpn - 1;
		} else if (rpn[rpn_i] == '+') {
			stack[stack_len - 2] += stack[stack_len - 1];
			stack_len--;
		} else if (rpn[rpn_i] == '-') {
			stack[stack_len - 2] -= stack[stack_len - 1];
			stack_len--;
		} else if (rpn[rpn_i] == '*') {
			stack[stack_len - 2] *= stack[stack_len - 1];
			stack_len--;
		} else if (rpn[rpn_i] == '/') {
			if (stack[stack_len - 1] == 0)
				error(10, "Division by zero.");
			stack[stack_len - 2] /= stack[stack_len - 1];
			stack_len--;
		}
	}

	/* Finalize */
	result = *stack;
	return result;
}

/* Converts infix notation to reverse polish notation */
static void
make_rpn(const char *in, char *rpn)
{
	/* Create and intialize a variables */
	size_t in_len,
   		in_i,
   		rpn_len = 0,
   		stack_len = 0;
	char stack[INTORPN_STACK_MAX_LEN];
	Element last_was = START;
	if (!in)
		error(20, "`in == NULL`.");
	in_len = strlen(in);
	if (in_len == 0)
		error(30, "Invalid syntax.");
	if (!rpn)
		error(21, "`rpn == NULL`.");

	/* Build a RPN */
	for (in_i = 0; in_i < in_len; ++in_i) {
		if (
			(isdigit(in[in_i]) && last_was != CLOSING_BRACKET)
			|| (
				/* Negative number */
				in[in_i] == '-'
				&& in_i + 1 < in_len
				&& isdigit(in[in_i + 1])
				&& last_was != NUMBER
				&& last_was != OP
		&& last_was != CLOSING_BRACKET
			)
		) {
			if (in[in_i] == '-')
				rpn[rpn_len++] = in[in_i++];
			for (; isdigit(in[in_i]); in_i++)
				rpn[rpn_len++] = in[in_i];
			in_i--;
			rpn[rpn_len++] = ' ';
			last_was = NUMBER;
		} else if (in[in_i] == '(') {
			if (last_was == NUMBER)
				error(31, "Invalid syntax.");
			stack[stack_len++] = '(';
			last_was = OPENING_BRACKET;
		} else if (in[in_i] == ')') {
			if (last_was == OP || last_was == OPENING_BRACKET)
				error(32, "Invalid syntax.");
			for (; stack_len > 0 && stack[stack_len - 1] != '('; stack_len--) {
				rpn[rpn_len++] = stack[stack_len - 1];
				rpn[rpn_len++] = ' ';
			}
			if (stack_len == 0)
				/* If opening bracket was not found */
				error(33, "Invalid syntax.");
			else
				stack_len--;
			last_was = CLOSING_BRACKET;
		} else if (op_prior(in[in_i])) {
			if (last_was == OP)
				error(34, "Invalid syntax.");
			while (
				stack_len > 0
				&& op_prior(stack[stack_len - 1]) >= op_prior(in[in_i])
			) {
				rpn[rpn_len++] = stack[--stack_len];
				rpn[rpn_len++] = ' ';
			}
			stack[stack_len++] = in[in_i];
			last_was = OP;
		} else {
			error(35, "Invalid syntax.");
		}
	}
	if (last_was == OP)
		error(36, "Invalid syntax.");
	while (stack_len > 0) {
		if (!op_prior(stack[stack_len - 1]))
			error(37, "Invalid syntax.");
		rpn[rpn_len++] = stack[--stack_len];
		rpn[rpn_len++] = ' ';
	}
	if (rpn_len > 0)
		/* Remove closing space */
		rpn[--rpn_len] = '\0';
}

/* Returns operation's priority or `0` if `op` is not operation. */
static unsigned char
op_prior(char op)
{
	switch (op) {
	case '+': /* FALLTHROUGH */
	case '-':
		return 1;
	case '*': /* FALLTHROUGH */
	case '/':
		return 2;
	default:
		return 0;
	}
}
