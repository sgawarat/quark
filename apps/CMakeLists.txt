# SPDX-FileCopyrightText: 2021 sgawarat <sgawarat@gmail.com>
# SPDX-License-Identifier: GPL-2.0-or-later

if(LINUX)
  add_subdirectory(linux)
endif()

if(WIN32)
  add_subdirectory(win32)
endif()

if(QUARK_X11)
  add_subdirectory(x11)
endif()
