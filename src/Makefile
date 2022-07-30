CC = gcc
CFLAGS = -Wall -g

SRCS = main.c token.c laxer.c io.c list.c parser.c ast.c code_generator.c symtable.c reg_stack.c type.c tc.c str_lit.c function_frame.c
OBJS = $(SRCS:%.c=%.o)
TARGET = ../bindi

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clear:
	rm $(OBJS) $(TARGET)
