add_executable(runtime_smoke
  runtime_smoke.cpp
)

target_link_libraries(runtime_smoke
  PRIVATE
    shakti_runtime
)

add_executable(backend_selection_smoke
  backend_selection_smoke.cpp
)

target_link_libraries(backend_selection_smoke
  PRIVATE
    shakti_runtime
)
