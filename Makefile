# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: bregneau <bregneau@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/03/01 19:17:00 by bregneau          #+#    #+#              #
#    Updated: 2023/03/06 18:49:05 by bregneau         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME			=	ircserv

INC				=	-MMD -I inc/

SRC_PATH		=	src/
OBJ_PATH		=	obj/

OBJ_DIRS		=	$(OBJ_PATH)

SRC				=	main.cpp \
					Server.cpp \
					utils.cpp \
					Client.cpp

OBJ				=	$(addprefix $(OBJ_PATH),$(SRC:.cpp=.o))
DEPS			=	$(addprefix $(OBJ_PATH),$(SRC:.cpp=.d))

CXX				=	c++
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
