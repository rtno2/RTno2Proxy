set(INCLUDE_DIR ../include)

set(rtno_main_srcs rtno_main.cpp)
set(com2tcp_main_srcs com2tcp_main.cpp)
set(rtno_test_srcs rtno_test.cpp)
set(rtno_hal_srcs
  ${INCLUDE_DIR}/hal/EtherTcp.h
  ${INCLUDE_DIR}/hal/libysuga.h
  ${INCLUDE_DIR}/hal/Serial.h
  ${INCLUDE_DIR}/hal/SerialDevice.h
  ${INCLUDE_DIR}/hal/SerialPort.h
  ${INCLUDE_DIR}/hal/ServerSocket.h
  ${INCLUDE_DIR}/hal/Socket.h
  hal/EtherTcp.cpp
  hal/Serial.cpp
  hal/SerialPort.cpp
)
set(rtno_srcs
  ${INCLUDE_DIR}/rtno2/command.h
  ${INCLUDE_DIR}/rtno2/connection_type.h
  ${INCLUDE_DIR}/rtno2/definition.h
  ${INCLUDE_DIR}/rtno2/ec_type.h
  ${INCLUDE_DIR}/rtno2/logger.h
  ${INCLUDE_DIR}/rtno2/packet.h
  ${INCLUDE_DIR}/rtno2/protocol.h
  ${INCLUDE_DIR}/rtno2/result.h
  ${INCLUDE_DIR}/rtno2/state.h
  ${INCLUDE_DIR}/rtno2/transport.h
  ${INCLUDE_DIR}/rtno2/type_code.h
  rtno2proxy/transport.cpp
  rtno2proxy/packet.cpp
  rtno2proxy/protocol.cpp
  rtno2proxy/logger.cpp
)

add_library(rtno_halStatic STATIC ${rtno_hal_srcs})
add_library(rtno_hal SHARED ${rtno_hal_srcs})

add_library(rtno_proxyStatic STATIC ${rtno_srcs})
add_library(rtno_proxy SHARED ${rtno_srcs})

add_executable(rtno2 ${rtno_main_srcs})
target_link_libraries(rtno2 rtno_halStatic rtno_proxyStatic)

add_executable(com2tcp ${com2tcp_main_srcs})
target_link_libraries(com2tcp rtno_halStatic )

add_executable(rtno_test ${rtno_test_srcs})
target_link_libraries(rtno_test rtno_halStatic rtno_proxyStatic)