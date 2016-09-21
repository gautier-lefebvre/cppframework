DISTDIR=    ../dist/
NAME=       $(DISTDIR)cppframework
DLLNAME=		$(NAME).so
RULENAME=   cppframework

INCLDIR=    include/
SRCDIR=     src/

LIBRARYDIR= $(SRCDIR)Library/
LIBRARYSRC= $(LIBRARYDIR)Collection/ByteArray.cpp                 \
            $(LIBRARYDIR)Encryption/XOR.cpp                       \
            $(LIBRARYDIR)Factory/Exception.cpp                    \
            $(LIBRARYDIR)Network/CURL/EasyHandle.cpp              \
            $(LIBRARYDIR)Network/CURL/MultiHandle.cpp             \
            $(LIBRARYDIR)Network/CURL/Exception.cpp               \
            $(LIBRARYDIR)Property/AEndable.cpp                    \
            $(LIBRARYDIR)Property/Initializable.cpp               \
            $(LIBRARYDIR)ThirdParty/cppformat/format.cpp          \
            $(LIBRARYDIR)ThirdParty/cppformat/posix.cpp           \
            $(LIBRARYDIR)Threading/Lockable.cpp                   \
            $(LIBRARYDIR)Threading/Notifiable.cpp                 \
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
            $(COREDIR)Network/Exception.cpp                       \
            $(COREDIR)Network/NetworkManager.cpp                  \
            $(COREDIR)Network/Http/AHttpMessage.cpp               \
            $(COREDIR)Network/Http/HttpClient.cpp                 \
            $(COREDIR)Network/Http/HttpConnection.cpp             \
            $(COREDIR)Network/Http/HttpRequest.cpp                \
            $(COREDIR)Network/Http/HttpResponse.cpp               \
            $(COREDIR)Network/Http/HttpPipeliningConnection.cpp   \
            $(COREDIR)Network/Tcp/TcpClient.cpp                   \
            $(COREDIR)Network/Tcp/TcpManager.cpp                  \
            $(COREDIR)Network/Tcp/TcpServer.cpp                   \
            $(COREDIR)Network/Tcp/TcpSocket.cpp                   \
            $(COREDIR)Network/Tcp/TcpSocketStream.cpp             \
            $(COREDIR)Network/Udp/AUdpSocket.cpp                  \
            $(COREDIR)Network/Udp/AUdpSocketIO.cpp                \
            $(COREDIR)Network/Udp/UdpClient.cpp                   \
            $(COREDIR)Network/Udp/UdpManager.cpp                  \
            $(COREDIR)Network/Udp/UdpServer.cpp                   \
            $(COREDIR)Network/Udp/UdpSocketClient.cpp             \
            $(COREDIR)Network/Udp/UdpSocketServer.cpp             \
            $(COREDIR)Network/Udp/UdpSocketStream.cpp             \
            $(COREDIR)Worker/DelayedTasksThread.cpp               \
            $(COREDIR)Worker/WorkerManager.cpp                    \
            $(COREDIR)Worker/Task.cpp                             \
            $(COREDIR)Worker/WorkerThread.cpp

SRCDLL=     $(LIBRARYSRC)                                         \
            $(CORESRC)

SRCPROG=    $(SRCDLL)                                             \
            $(SRCDIR)App/main.cpp

OBJDLL=     $(SRCDLL:.cpp=.o)
OBJPROG=    $(SRCPROG:.cpp=.o)

DEPS=       $(OBJPROG:.o=.d)

LDPTHREAD=  -lpthread
LDFLAGS=    $(LDPTHREAD) -lssl -lcrypto -lcurl

CXXFLAGS=   -I$(INCLDIR) -std=c++11 -O6 -W -Wall -Wextra -Werror

CXX=        g++

all:         $(RULENAME)

$(RULENAME): dll

debug:       CXXFLAGS += -D__DEBUG__
debug:       prog

nodebug:     prog

prog:        headerify $(OBJPROG) create_dist
			@$(CXX) -o $(NAME) $(OBJPROG) $(LDFLAGS) && echo "Built    $(NAME)"

dll:         CXXFLAGS += -c -fPIC
dll:         headerify $(OBJDLL) create_dist
			@$(CXX) -shared -o $(DLLNAME) $(OBJDLL) $(LDFLAGS) && echo "Built    $(DLLNAME)"

headerify:
			python3 headerify.py

create_dist:
			mkdir -p $(DISTDIR)

doc:
			doxygen

%.o:         %.cpp
			@$(CXX) -c $(CXXFLAGS) -MMD $< -o $@ -MF $(patsubst %.o, %.d, $@) && echo "Compiled $<"

clean:
			@rm -f $(OBJPROG) && echo "Objects removed"

clean_dep:
			@rm -f $(DEPS) && echo "Dependencies removed"

fclean:      clean clean_dep
			@rm -f $(NAME) $(DLLNAME) && echo "Executable removed"

re:          fclean all

-include $(DEPS)
