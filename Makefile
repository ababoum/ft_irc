NAME			=	ircserv

INC				=	-MMD -I inc/

SRC_PATH		=	src/
OBJ_PATH		=	obj/

OBJ_DIRS		=	$(OBJ_PATH)

SRC				=	main.cpp \
					Server.cpp \
					Server_replies.cpp \
					utils.cpp \
					Client.cpp \
					Channel.cpp

OBJ				=	$(addprefix $(OBJ_PATH),$(SRC:.cpp=.o))
DEPS			=	$(addprefix $(OBJ_PATH),$(SRC:.cpp=.d))

CXX				=	g++
CXXFLAGS		=	-Wall -Wextra -Werror -std=c++98 -g

all:				$(NAME)

$(OBJ_DIRS):
					@mkdir -p $(OBJ_DIRS)
					
$(OBJ_PATH)%.o:		$(SRC_PATH)%.cpp
					$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

$(NAME):			$(OBJ_DIRS) $(OBJ)
					$(CXX) $(CXXFLAGS) $(OBJ) -o $@

clean:				
					rm -rf $(OBJ_PATH)

fclean:				clean
					rm -f $(NAME)

re:					fclean
					make all
					
-include			$(DEPS)

.PHONY:				all clean fclean re
