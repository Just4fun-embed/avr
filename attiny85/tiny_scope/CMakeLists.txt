cmake_minimum_required(VERSION 3.28)

project(oscilloscope CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE PROJECT_SRC 
    src/*.cpp)

add_executable(${PROJECT_NAME} ${PROJECT_SRC})

target_include_directories(${PROJECT_NAME} PRIVATE
    ./inc)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    F_CPU=1000000)

set(CPU_PARAM 
    -mmcu=attiny85)

target_compile_options(${PROJECT_NAME} PRIVATE
    ${CPU_PARAM}
    -ffunction-sections
    -fdata-sections
    -Og -g2
    -Wall)

target_link_options(${PROJECT_NAME} PRIVATE
    ${CPU_PARAM}
    -Wl,-Map=${PROJECT_NAME}.map
    -Wl,--start-group
    -Wl,-lm
    -Wl,--end-group
    -Wl,--gc-sections)
    
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${PROJECT_NAME}>
    COMMAND ${CMAKE_OBJDUMP} -h -S ${PROJECT_NAME} > ${PROJECT_NAME}.lss
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${PROJECT_NAME}> ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}> ${PROJECT_NAME}.bin)
    
