
NAME = philo

CFLAGS = -Wall -Wextra -Werror

SRCS =	actions.c \
		check.c \
		clean.c \
		initialize.c \
		main.c \
		print.c \
		routine.c \

OBJS = $(SRCS:.c=.o)

all: $(NAME)

$(NAME) : $(OBJS)
	cc $(CFLAGS) $(OBJS) -o $(NAME)

clean: 
	rm -rf $(OBJS)

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: all re clean fclean