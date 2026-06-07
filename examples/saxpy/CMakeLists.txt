add_executable(saxpy
  saxpy.cpp
)

target_link_libraries(saxpy
  PRIVATE
    shakti_runtime
)
