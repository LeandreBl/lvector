
NAME	= tests_lvector

CC	= gcc

SRCS	= tests/vector.c

OBJS	= $(SRCS:.c=.o)

RM	= rm -f

CFLAGS	+= -Werror -Wall -Wextra -pedantic
CFLAGS	+= -I.
LDFLAGS += -lcriterion

GREEN	= '\033[0;32m'
NO_COLOR	= '\033[0m'

%.o : %.c
	@$ $(CC) $(CFLAGS) -c $< -o $@
	@echo "$(CC) $(CFLAGS) -c $< -o $@ ["$(GREEN)"OK"$(NO_COLOR)"]"
.SUFFIXES: .o .c

tests_run: $(OBJS)
	@$ $(CC) $(OBJS) $(LDFLAGS) -o $(NAME)
	@echo "$(CC) $(OBJS) $(LDFLAGS) -o $(NAME) \
	["$(GREEN)"LINKING OK"$(NO_COLOR)"]"
	./$(NAME)

val_run: CFLAGS += -g3
val_run: $(OBJS)
	@$ $(CC) $(OBJS) $(LDFLAGS) -o $(NAME)
	@echo "$(CC) $(OBJS) $(LDFLAGS) -o $(NAME) \
	["$(GREEN)"LINKING OK"$(NO_COLOR)"]"
	valgrind --trace-children=yes -q ./$(NAME)

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

.PHONY: tests_run val_run clean fclean