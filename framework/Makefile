NAME=			cppframework.so

SRCDIR=			sources/

LIBRARYDIR=		$(SRCDIR)Library/
LIBRARYSRC=		$(LIBRARYDIR)Collection/ByteArray.cpp			\
				$(LIBRARYDIR)Encryption/MD5.cpp					\
				$(LIBRARYDIR)Encryption/XOR.cpp					\
				$(LIBRARYDIR)Factory/AFactored.cpp				\
				$(LIBRARYDIR)ThirdParty/json.cpp				\
				$(LIBRARYDIR)Threading/Condition.cpp			\
				$(LIBRARYDIR)Threading/Lock.cpp					\
				$(LIBRARYDIR)Tool/Arguments.cpp					\
				$(LIBRARYDIR)Tool/Date.cpp						\
				$(LIBRARYDIR)Tool/Logger.cpp					\
				$(LIBRARYDIR)Tool/Random.cpp					\
				$(LIBRARYDIR)Tool/String.cpp					\
				$(LIBRARYDIR)Tool/TypeChecker.cpp				\
				$(LIBRARYDIR)Exception.cpp

COREDIR=		$(SRCDIR)Core/
CORESRC=		$(COREDIR)Exception.cpp							\
				$(COREDIR)Network/Exception.cpp					\
				$(COREDIR)Network/TCP/Socket.cpp				\
				$(COREDIR)Network/TCP/SocketStream.cpp			\
				$(COREDIR)Network/UDP/SocketClient.cpp

SRC=			$(LIBRARYSRC)									\
				$(CORESRC)										\
				$(SRCDIR)App/main.cpp

OBJ=			$(SRC:.cpp=.o)

DEPS=			$(OBJ:.o=.d)

LDPTHREAD=		-lpthread
LDFLAGS=		$(LDPTHREAD) -lssl -lcrypto -lcurl

CXXFLAGS=		-Iheaders/ -std=c++11 -W -Wall -Wextra -Werror
CXXFLAGS+=		-c -fPIC # dynamic library

CXX=			g++

all:			$(NAME)

$(NAME):		debug

debug:			CXXFLAGS += -D__DEBUG__
debug:			compile

nodebug:		compile

compile:		$(OBJ)
				@$(CXX) -shared -o $(NAME) $(OBJ) $(LDFLAGS) && echo "Built    $(NAME)"


%.o:			%.cpp
				@$(CXX) -c $(CXXFLAGS) -MMD $< -o $@ -MF $(patsubst %.o, %.d, $@) && echo "Compiled $<"

clean:			
				@rm -f $(OBJ) && echo "Objects removed"

clean_dep:
				@rm -f $(DEPS) && echo "Dependencies removed"

fclean:			clean clean_dep
				@rm -f $(NAME) && echo "Executable removed"

re:				fclean all

-include $(DEPS)
