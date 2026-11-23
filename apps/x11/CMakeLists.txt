# SPDX-FileCopyrightText: 2026 sgawarat <sgawarat@gmail.com>
# SPDX-License-Identifier: GPL-2.0-or-later

find_package(X11)

if(X11_FOUND)
  add_subdirectory(keytest)
endif()
