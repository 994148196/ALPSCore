#  Copyright Matthias Troyer and Synge Todo 2009 - 2010.
#  Distributed under the Boost Software License, Version 1.0.
#      (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

include_directories(${PROJECT_BINARY_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${Boost_ROOT_DIR})

IF(NOT ALPS_LLVM_WORKAROUND)
  FOREACH (name complexobservable detailedbinning histogram histogram2 signed simpleobseval testobservableset observableset_hdf5 observableset_xml vectorobseval mcdata mcdata2 mcanalyze mergeobs_plus mergeobs_plus2 mergeobs_times mergeobs_times2 mergeobs_divide mergeobs_log)
    add_executable(${name} ${name}.C)
    add_dependencies(${name} alps)
    target_link_libraries(${name} alps)
    add_alps_test(${name})
  ENDFOREACH(name)
ENDIF(NOT ALPS_LLVM_WORKAROUND)

add_executable(dumpbench dumpbench.C)
target_link_libraries(dumpbench alps)
