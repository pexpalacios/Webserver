MAKEFLAGS += --no-print-directory

NAME = WebServer

FLAGS = -Wall -Wextra -Werror -std=c++98

DIR_SRC = srcs
DIR_OBJ = build/obj
DIR_INC = includes

PARSING = ConfigParser LocationConfig ServerConfig

SRC_FILES = $(addsuffix .cpp, $(addprefix srcs/parse/, $(PARSING))) \
			srcs/main.cpp

OBJS = $(patsubst srcs/%.cpp, build/obj/%.o, $(SRC_FILES))

##################

all: $(NAME)

$(DIR_OBJ):
	mkdir -p $(DIR_OBJ)

$(NAME): $(OBJS)
	c++ $(FLAGS) -o $(NAME) $(OBJS)

$(DIR_OBJ)/%.o: $(DIR_SRC)/%.cpp
	mkdir -p $(dir $@)
	c++ $(FLAGS) -c $< -o $@

clean: 
	rm -rf build

fclean: clean
	rm -f $(NAME)

re: fclean all

-PHONY: all clean fclean re