find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})

add_definitions(${GTK3_CFLAGS_OTHER} -rdynamic)
add_executable(main main.c)
add_subdirectory(btree-al bt)
target_link_libraries(main ${GTK3_LIBRARIES} bt)