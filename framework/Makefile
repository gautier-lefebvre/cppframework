NAME=       cppframework

INCLDIR=    include/
SRCDIR=     src/

LIBRARYDIR= $(SRCDIR)Library/
LIBRARYSRC= $(LIBRARYDIR)Collection/ByteArray.cpp                 \
            $(LIBRARYDIR)Encryption/MD5.cpp                       \
            $(LIBRARYDIR)Encryption/XOR.cpp                       \
            $(LIBRARYDIR)Factory/AFactored.cpp                    \
            $(LIBRARYDIR)Factory/Exception.cpp                    \
            $(LIBRARYDIR)Network/CURL/EasyHandle.cpp              \
            $(LIBRARYDIR)Network/CURL/MultiHandle.cpp             \
            $(LIBRARYDIR)Network/CURL/Exception.cpp               \
            $(LIBRARYDIR)Property/AEndable.cpp                    \
            $(LIBRARYDIR)Property/Initializable.cpp               \
            $(LIBRARYDIR)ThirdParty/cppformat/format.cpp          \
            $(LIBRARYDIR)ThirdParty/cppformat/posix.cpp           \
            $(LIBRARYDIR)Threading/Condition.cpp                  \
            $(LIBRARYDIR)Threading/Lock.cpp                       \
            $(LIBRARYDIR)Tool/Arguments.cpp                       \
            $(LIBRARYDIR)Tool/Date.cpp                            \
            $(LIBRARYDIR)Tool/Logger.cpp                          \
            $(LIBRARYDIR)Tool/Random.cpp                          \
            $(LIBRARYDIR)Tool/Signal.cpp                          \
            $(LIBRARYDIR)Tool/String.cpp                          \
            $(LIBRARYDIR)Tool/TypeChecker.cpp                     \
            $(LIBRARYDIR)Exception.cpp

COREDIR=    $(SRCDIR)Core/
CORESRC=    $(COREDIR)Exception.cpp                               \
            $(COREDIR)FactoryManager.cpp                          \
            $(COREDIR)System.cpp                                  \
            $(COREDIR)Event/Exception.cpp                         \
            $(COREDIR)Event/EventHandle.cpp                       \
            $(COREDIR)Event/EventInfo.cpp                         \
            $(COREDIR)Event/EventManager.cpp                      \
            $(COREDIR)Network/Exception.cpp                       \
            $(COREDIR)Network/NetworkManager.cpp                  \
            $(COREDIR)Network/Http/AHttpMessage.cpp               \
            $(COREDIR)Network/Http/HttpClient.cpp                 \
            $(COREDIR)Network/Http/HttpConnection.cpp             \
            $(COREDIR)Network/Http/HttpRequest.cpp                \
            $(COREDIR)Network/Http/HttpResponse.cpp               \
            $(COREDIR)Network/Http/HttpPipeliningConnection.cpp   \
            $(COREDIR)Network/Tcp/TcpSocket.cpp                   \
            $(COREDIR)Network/Tcp/TcpSocketStream.cpp             \
            $(COREDIR)Network/Tcp/TcpManager.cpp                  \
            $(COREDIR)Network/Udp/AUdpSocket.cpp                  \
            $(COREDIR)Network/Udp/AUdpSocketIO.cpp                \
            $(COREDIR)Network/Udp/UdpSocketServer.cpp             \
            $(COREDIR)Network/Udp/UdpSocketClient.cpp             \
            $(COREDIR)Network/Udp/UdpSocketStream.cpp             \
            $(COREDIR)Network/Udp/UdpManager.cpp                  \
            $(COREDIR)Worker/DelayedTasksThread.cpp               \
            $(COREDIR)Worker/WorkerManager.cpp                    \
            $(COREDIR)Worker/Task.cpp                             \
            $(COREDIR)Worker/WorkerThread.cpp

SRC=        $(LIBRARYSRC)                                         \
            $(CORESRC)                                            \
            $(SRCDIR)App/main.cpp

OBJ=        $(SRC:.cpp=.o)

DEPS=       $(OBJ:.o=.d)

LDPTHREAD=  -lpthread
LDFLAGS=    $(LDPTHREAD) -lssl -lcrypto -lcurl

CXXFLAGS=   -I$(INCLDIR) -std=c++11 -W -Wall -Wextra -Werror

CXX=        g++

all:        $(NAME)

$(NAME):    debug

debug:      CXXFLAGS += -D__DEBUG__
debug:      compile

nodebug:    compile

compile:    $(OBJ)
			@$(CXX) -o $(NAME) $(OBJ) $(LDFLAGS) && echo "Built    $(NAME)"

dll:		CXXFLAGS += -c -fPIC
dll:		$(OBJ)
			@$(CXX) -shared -o $(NAME).so $(OBJ) $(LDFLAGS) && echo "Built    $(NAME)"

%.o:        %.cpp
			@$(CXX) -c $(CXXFLAGS) -MMD $< -o $@ -MF $(patsubst %.o, %.d, $@) && echo "Compiled $<"

clean:
			@rm -f $(OBJ) && echo "Objects removed"

clean_dep:
			@rm -f $(DEPS) && echo "Dependencies removed"

fclean:     clean clean_dep
			@rm -f $(NAME) && echo "Executable removed"

re:         fclean all

-include $(DEPS)
