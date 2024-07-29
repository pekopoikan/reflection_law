# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/r/Desktop/cpp/test/build/_deps/imgui_external-src"
  "/home/r/Desktop/cpp/test/build/_deps/imgui_external-build"
  "/home/r/Desktop/cpp/test/build/_deps/imgui_external-subbuild/imgui_external-populate-prefix"
  "/home/r/Desktop/cpp/test/build/_deps/imgui_external-subbuild/imgui_external-populate-prefix/tmp"
  "/home/r/Desktop/cpp/test/build/_deps/imgui_external-subbuild/imgui_external-populate-prefix/src/imgui_external-populate-stamp"
  "/home/r/Desktop/cpp/test/build/_deps/imgui_external-subbuild/imgui_external-populate-prefix/src"
  "/home/r/Desktop/cpp/test/build/_deps/imgui_external-subbuild/imgui_external-populate-prefix/src/imgui_external-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/r/Desktop/cpp/test/build/_deps/imgui_external-subbuild/imgui_external-populate-prefix/src/imgui_external-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/r/Desktop/cpp/test/build/_deps/imgui_external-subbuild/imgui_external-populate-prefix/src/imgui_external-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
