# SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
# SPDX-License-Identifier: GPL-2.0-or-later

find_package(doctest REQUIRED)
include(doctest)

add_executable(tests
  main.cpp
  bitset_test.cpp
)
target_link_libraries(tests PRIVATE
  config
  doctest::doctest
)

doctest_discover_tests(tests)
