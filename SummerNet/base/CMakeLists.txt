set(base_SRCS
    CountDownLatch.cpp
    Logging.cpp
    TimerFd.cpp
    )

add_library(sumnet_base ${base_SRCS})
target_link_libraries(sumnet_base ${Glog_LIBRARIES} pthread)

install(TARGETS sumnet_base DESTINATION lib)

file(GLOB HEADERS "*.h")
install(FILES ${HEADERS} DESTINATION include/SummerNet/base)
