add_library(keyboard STATIC
    keymap.cpp
)
target_compile_definitions(keyboard PUBLIC
    ACTIONMAP_ENABLE
)
target_link_libraries(keyboard PRIVATE
    config
)
