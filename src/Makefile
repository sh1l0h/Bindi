CC = gcc
CFLAGS = -Wall

SRCS = main.c token.c laxer.c io.c list.c parser.c ast.c code_generator.c symtable.c reg_stack.c type.c tc.c
OBJS = $(SRCS:%.c=%.o)
TARGET = ../bindi

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -g $(OBJS) $(CFLAGS) -o $(TARGET)

%.o: %.c
	${CC} -g -c $< -o $@

clear:
	rm *.o ../bindi
