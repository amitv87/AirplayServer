cmake_minimum_required(VERSION 3.4.1)

project(AirplayReciever VERSION 0.1.0 LANGUAGES CXX C)

add_subdirectory(lib/crypto)
add_subdirectory(lib/curve25519)
add_subdirectory(lib/ed25519)
add_subdirectory(lib/playfair)
add_subdirectory(lib/plist)
add_subdirectory(lib/fdk-aac)
add_subdirectory(lib)
add_subdirectory(mDNSResponder)

add_executable(mirror_recv "main.c")
target_link_libraries(mirror_recv -lpthread -lstdc++ -lm play-lib jdns_sd)
