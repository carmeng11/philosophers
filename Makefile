NAME = philo
CC = gcc
CFLAGS = -Wall -Wextra -Werror -I./includes -g3 -fsanitize=thread -pthread

SRCS = src/main.c src/init.c src/philo_routine.c src/utils.c src/monitor.c \
		src/actions.c
OBJS = $(SRCS:.c=.o)

all:  $(NAME)


$(NAME): $(OBJS)

	$(CC) $(CFLAGS) -o $(NAME) $(OBJS) $(LIBS)

%.o: %.c includes/philo.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re