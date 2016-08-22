set(net_SRCS
	EventLoop.cpp
	Channel.cpp
	Poller.cpp
	poller/PollPoller.cpp
	TimerId.cpp
	TimerQueue.cpp
	SocketOps.cpp
	InetAddress.cpp
	Socket.cpp
	Acceptor.cpp
	Buffer.cpp
	TcpConnection.cpp
	TcpServer.cpp
	EventLoopThread.cpp
	EventLoopThreadPool.cpp
	)

add_library(sumnet ${net_SRCS})
target_link_libraries(sumnet sumnet_base boost_system)

install(TARGETS sumnet DESTINATION lib)

set(HEADERS
	EventLoop.h
	)

install(FILES ${HEADERS} DESTINATION include/muduo/net)

if(CMAKE_BUILD_EXAMPLE)
	add_subdirectory(example)
endif()
