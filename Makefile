NAME		:= webserver
CXX			:= g++
CXXFLAGS	:= -Wall -Wextra -Werror -std=c++98
RM			:= rm -f

SRCS := srcs/main.cpp \
		srcs/Server.cpp \
		srcs/Server.run.cpp \
		srcs/Signal.cpp \
		srcs/parse/ConfigParser.cpp \
		srcs/parse/LocationConfig.cpp \
		srcs/parse/ServerConfig.cpp \
		srcs/parse/ValueParser.cpp
OBJS	:= $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	@echo "Linking $(NAME)..."
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)
	@echo "✅ Build complete."

%.o: %.cpp
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@


up: all

clean:
	@echo "Cleaning object files..."
	$(RM) $(OBJS)

fclean: clean
	@echo "Removing executable..."
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re up
