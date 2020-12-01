cmake_minimum_required(VERSION 3.7)

set(OPENSSL_USE_STATIC_LIBS TRUE)

find_package(OpenSSL REQUIRED)

# dl required for static linking with openssl
set(OPENSSL_LIBRARIES ${OPENSSL_LIBRARIES} -ldl)

add_executable(keygen keygen.cpp)

target_link_libraries(keygen sysinfo ${OPENSSL_LIBRARIES})

