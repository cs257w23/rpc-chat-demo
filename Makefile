default: chat 

chat: main.cpp chat.o rpclib/build/librpc.a
	g++ main.cpp chat.o -std=c++20 -I rpclib/include -L rpclib/build -lrpc -o chat

chat.o: chat.cpp chat.h rpclib
	g++ chat.cpp -std=c++20 -I rpclib/include -c


rpclib/build/librpc.a: rpclib
	cd rpclib \
		&& mkdir -p build \
		&& cd build \
		&& cmake .. \
		&& make -j4

rpclib:
	git clone https://github.com/rpclib/rpclib \
		&& cd rpclib \
		&& git checkout 463887f16ffcc4cf9ee0421bf25d49165b5e36f9

.PHONY: clean
clean:
	rm -f chat
	rm -f chat.o
	rm -fr rpclib/build
