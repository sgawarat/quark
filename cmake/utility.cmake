# SPDX-FileCopyrightText: 2021-2026 sgawarat <sgawarat@gmail.com>
# SPDX-License-Identifier: GPL-2.0-or-later
include_guard(DIRECTORY)

# Quantumのルールに従ってファイルを探す
function(find_quantum_files filename pre_var post_var)
  set(pre "")
  set(post "")
  cmake_path(SET dir NORMALIZE "${QUARK_KEYMAP_DIR}")
  cmake_path(SET goal_dir NORMALIZE "${CMAKE_SOURCE_DIR}/keyboards")

  # filenameがパスでないことを確認する
  if(filename MATCHES "[/\\\\]")
    message(FATAL_ERROR "'${filename}' is not filename")
  endif()

  # たどるパスがkeyboardsの中にあることを確認する
  cmake_path(IS_PREFIX goal_dir ${dir} NORMALIZE is_prefix)
  if(NOT is_prefix)
    message(FATAL_ERROR "Keymap '${dir}' is not found in '${goal_dir}'")
  endif()

  # 終点のディレクトリに到達するまで、<filename>とpost_<filename>という名前のファイルを列挙する
  while(NOT ${dir} STREQUAL ${goal_dir})
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

    # 念のため無限ループに陥らないように、ルートディレクトリに到達したか確認する
    if(dir STREQUAL parent_dir)
      message(FATAL_ERROR "Root directory was detected")
    endif()

    # ひとつ上のディレクトリに移動する
    set(dir "${parent_dir}")
  endwhile()

  # 結果を返す
  set(${pre_var} ${pre} PARENT_SCOPE)
  set(${post_var} ${post} PARENT_SCOPE)
endfunction()
