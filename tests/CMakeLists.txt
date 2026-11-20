# SPDX-FileCopyrightText: 2026 sgawarat <sgawarat@gmail.com>
# SPDX-License-Identifier: GPL-2.0-or-later

find_package(doctest CONFIG REQUIRED)
include(doctest)

add_library(doctest_main OBJECT
  main.cpp
)
target_link_libraries(doctest_main PRIVATE
  config
  doctest::doctest
)

add_executable(tests
  bitset_test.cpp
)

if(MSVC)
  target_compile_options(tests PRIVATE
    "SHELL:/FI doctest/doctest.h"
  )
else()
  target_compile_options(tests PRIVATE
    "SHELL:-include doctest/doctest.h"
  )
endif()

target_link_libraries(tests PRIVATE
  config
  doctest::doctest
  doctest_main
)

doctest_discover_tests(tests)
