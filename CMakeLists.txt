#1.cmake verson，指定cmake版本
cmake_minimum_required(VERSION 3.14)

#2.project name，指定项目的名称，一般和项目的文件夹名称对应
project(test C)

#3.head file path，头文件目录
INCLUDE_DIRECTORIES( lib )

#4.source directory，源文件目录
AUX_SOURCE_DIRECTORY(src DIR_SRCS)


#5.set environment variable，设置环境SET(TEST_MATH变量，编译用到的源文件全部都要放到这里，否则编译能够通过，但是执行的时候会出现各种问题，比如"symbol lookup error xxxxx , undefined symbol"
SET(TEST_MATH
${DIR_SRCS}
)

#ADD_EXECUTABLE(${PROJECT_NAME} ${TEST_MATH})
set(CMAKE_C_STANDARD 90)

#6.add executable file，添加要编译的可执行文件
#add_executable(${PROJECT_NAME} src/main.c )
add_executable(${PROJECT_NAME} src/XipYamlBase.c)

#find_library(LIBHELLO_PATH libyaml.a lib)
#IF(NOT LIBHELLO_PATH)
#    MESSAGE(FATAL_ERROR "libhello not found")
#ENDIF(NOT LIBHELLO_PATH)
#
#MESSAGE(SSSSSS ${LIBHELLO_PATH})


#TARGET_LINK_DIRECTORIES(${PROJECT_NAME}  ${PROJECT_SOURCE_DIR}/lib/)
#link_directories("${PROJECT_SOURCE_DIR}/lib")
#7.add link library，添加可执行文件所需要的库，比如我们用到了libm.so（命名规则：lib+name+.so），就添加该库的名称
#TARGET_LINK_LIBRARIES(${PROJECT_NAME}  libyaml.a)

#TARGET_LINK_LIBRARIES(${PROJECT_NAME}  ${LIBHELLO_PATH})

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/lib/libyaml.a)


#add_custom_target(test COMMAND make -f ${PROJECT_SOURCE_DIR}/src/makefile)
