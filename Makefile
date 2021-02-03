
NAME	= tests_lvector

CC	= gcc

SRCS	= tests/vector.c

OBJS	= $(SRCS:.c=.o)

RM	= rm -f

CFLAGS	+= -Werror -Wall -Wextra -g3
CFLAGS	+= -I./include
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

val_run: $(OBJS)
	@$ $(CC) $(OBJS) $(LDFLAGS) -o $(NAME)
	@echo "$(CC) $(OBJS) $(LDFLAGS) -o $(NAME) \
	["$(GREEN)"LINKING OK"$(NO_COLOR)"]"
	valgrind --trace-children=yes -q ./$(NAME)

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

install:
	@cp ./include/lvector.h /usr/local/include 2> /dev/null && \
	printf "\033[1m\033[32mLibrary successfull installed !\033[0m\n" || \
	printf "\033[1m\033[31mError : try sudo make install\033[0m\n"

static:
	@echo not implemented

.PHONY: tests_run val_run clean fclean install
