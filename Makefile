
NAME			:=  Matt_daemon

CXXFLAGS		:= -Wall -Werror -Wextra  -std=c++11

DEBUG			:= #-fsanitize=address -g3

OPTI			:= -O3

INC				:=	tintin_reporter.hpp \
					server.hpp \
					utils.hpp

INC_DIR			:= inc/

INC_FILES		:= $(addprefix $(INC_DIR), $(INC))

SRCS			:=	matt_daemon.cpp \
					tintin_reporter.cpp \
					server.cpp \
					utils.cpp

SRCS_DIR		:= src/

SRCS_FILES		:= $(addprefix $(SRCS_DIR), $(SRCS))

OBJS			:= $(SRCS:.cpp=.o)

OBJS_DIR		:= objs/

OBJS_FILES		:= $(addprefix $(OBJS_DIR), $(OBJS))

all:			$(NAME)

$(NAME):		$(OBJS_FILES)
				c++ $(CXXFLAGS) $(DEBUG) $(SRCS_FILES) -o $(NAME)

$(OBJS_DIR)%.o:	$(SRCS_DIR)%.cpp $(INC_FILES) Makefile
				mkdir -p $(OBJS_DIR)
				c++ $(CXXFLAGS) $(DEBUG) -c $< -o $@ -I$(INC_DIR)

clean:
				rm -rf $(OBJS_DIR)

fclean:			clean
				rm -f $(NAME)

re:				fclean all

.PHONY:			all clean fclean re 


