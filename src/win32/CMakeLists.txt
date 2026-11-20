# SPDX-FileCopyrightText: 2021 sgawarat <sgawarat@gmail.com>
# SPDX-License-Identifier: GPL-2.0-or-later

add_library(engine_impl STATIC
  injected.cpp
  key.cpp
  sink.cpp
  source.cpp
)
target_link_libraries(engine_impl PRIVATE
  quantum_config
)
