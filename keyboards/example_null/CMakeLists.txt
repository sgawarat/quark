add_library(keyboard STATIC
    keymap.c
)
target_link_libraries(keyboard PRIVATE
    config
    layout
)
