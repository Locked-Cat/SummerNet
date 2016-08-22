add_executable(DaytimeServer Daytime/Daytime.cpp Daytime/Main.cpp)
target_link_libraries(DaytimeServer sumnet)

add_executable(EchoServer Echo/Echo.cpp Echo/Main.cpp)
target_link_libraries(EchoServer sumnet)

add_executable(EchoServerMT EchoMT/EchoMT.cpp EchoMT/Main.cpp)
target_link_libraries(EchoServerMT sumnet)