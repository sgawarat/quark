# SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
# SPDX-License-Identifier: GPL-2.0-or-later
include_guard(DIRECTORY)

# Quantumのルールに従ってファイルを探す
function(find_quantum_files filename pre_var post_var)
  set(pre "")
  set(post "")
  set(dir "${QUARK_KEYMAP_DIR}")

  # 終点のディレクトリに到達するまで、<filename>とpost_<filename>という名前のファイルを列挙する
  while(NOT dir STREQUAL "${CMAKE_SOURCE_DIR}/keyboards")
    # <filename>はディレクトリの浅い順に並べる
    if(EXISTS "${dir}/${filename}")
      list(PREPEND pre "${dir}/${filename}")
    endif()

    # post_<filename>はディレクトリの深い順に並べる
    if(EXISTS "${dir}/post_${filename}")
      list(APPEND post "${dir}/post_${filename}")
    endif()

    # ひとつ上のディレクトリを探す
    cmake_path(GET dir PARENT_PATH parent_dir)

    # ディレクトリを移動できなければ、そこはルートなので、glob処理を終了する
    if(dir STREQUAL parent_dir)
      break()
    endif()

    # ひとつ上のディレクトリに移動する
    set(dir "${parent_dir}")
  endwhile()

  # 結果を返す
  set(${pre_var} ${pre} PARENT_SCOPE)
  set(${post_var} ${post} PARENT_SCOPE)
endfunction()
