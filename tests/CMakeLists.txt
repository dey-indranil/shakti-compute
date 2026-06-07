add_executable(runtime_smoke
  runtime_smoke.cpp
)

target_link_libraries(runtime_smoke
  PRIVATE
    shakti_runtime
)

add_test(NAME runtime_smoke COMMAND runtime_smoke)

add_executable(backend_selection_smoke
  backend_selection_smoke.cpp
)

target_link_libraries(backend_selection_smoke
  PRIVATE
    shakti_runtime
)

add_test(NAME backend_selection_smoke COMMAND backend_selection_smoke)

add_executable(cuda_memory_smoke
  cuda_memory_smoke.cpp
)

target_link_libraries(cuda_memory_smoke
  PRIVATE
    shakti_runtime
)

add_test(NAME cuda_memory_smoke COMMAND cuda_memory_smoke)
