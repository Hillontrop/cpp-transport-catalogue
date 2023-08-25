cmake_minimum_required(VERSION 3.10)

project(PhoneBook CXX)
set(CMAKE_CXX_STANDARD 17)

find_package(Protobuf REQUIRED)
find_package(Threads REQUIRED)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS 
    transport_catalogue.proto
    map_renderer.proto
    svg.proto
    transport_router.proto
)

set(SRC_FILES 
    transport_router.h 
    router.h 
    ranges.h 
    graph.h 
    transport_router.cpp 
    svg.h 
    svg.cpp 
    request_handler.h 
    request_handler.cpp 
    map_renderer.h 
    map_renderer.cpp 
    domain.h 
    domain.cpp 
    geo.h 
    geo.cpp 
    json.h 
    json.cpp 
    json_builder.h 
    json_builder.cpp 
    json_reader.h 
    json_reader.cpp 
    serialization.h 
    serialization.cpp 
    main.cpp 
    transport_catalogue.h 
    transport_catalogue.cpp
    ${PROTO_SRCS} ${PROTO_HDRS}
)

add_executable(transport_catalogue ${SRC_FILES})
target_include_directories(transport_catalogue PUBLIC ${Protobuf_INCLUDE_DIRS})
target_include_directories(transport_catalogue PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(transport_catalogue ${Protobuf_LIBRARIES} Threads::Threads)