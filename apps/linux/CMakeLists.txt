# SPDX-FileCopyrightText: 2026 sgawarat <sgawarat@gmail.com>
# SPDX-License-Identifier: GPL-2.0-or-later

add_subdirectory(quark)

if(QUARK_QT)
  add_subdirectory(quark-ui)
endif()

add_subdirectory(keytest)
