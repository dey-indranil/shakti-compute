#include "shakti/shakti.h"

#include <cstdlib>
#include <cstring>
#include <iostream>

namespace {

bool expect(bool condition, const char* message) {
  if (condition) {
    return true;
  }

  std::cerr << "FAIL: " << message << "\n";
  return false;
}

bool lastErrorContains(const char* text) {
  const char* message = shaktiGetLastErrorMessage();
  return message != nullptr && std::strstr(message, text) != nullptr;
}

}  // namespace

int main() {
  bool ok = true;

  ok = ok && expect(shaktiGetLastErrorMessage() != nullptr,
                    "last error message is initially non-null");

  setenv("SHAKTI_BACKEND", "bogus", 1);
  void* ptr = reinterpret_cast<void*>(0x1);
  ok = ok && expect(shaktiMalloc(&ptr, 16) == SHAKTI_ERROR_INVALID_VALUE,
                    "unknown backend fails allocation");
  ok = ok && expect(ptr == nullptr, "unknown backend clears allocation output");
  ok = ok && expect(lastErrorContains("selected backend is unknown"),
                    "unknown backend sets detailed last error");

  setenv("SHAKTI_BACKEND", "cpu", 1);
  ok = ok && expect(shaktiMalloc(nullptr, 16) == SHAKTI_ERROR_INVALID_VALUE,
                    "null allocation output fails");
  ok = ok && expect(lastErrorContains("shaktiMalloc failed on backend 'cpu'"),
                    "backend invalid argument sets detailed last error");

  ok = ok && expect(shaktiMalloc(&ptr, 16) == SHAKTI_SUCCESS,
                    "successful allocation succeeds");
  ok = ok && expect(std::strcmp(shaktiGetLastErrorMessage(), "success") == 0,
                    "success clears last error detail");

  setenv("SHAKTI_BACKEND", "mock_gpu", 1);
  ok = ok && expect(shaktiFree(ptr) == SHAKTI_ERROR_INVALID_VALUE,
                    "wrong backend free fails");
  ok = ok && expect(lastErrorContains("allocation belongs to backend 'cpu'"),
                    "wrong backend free names allocation owner");
  ok = ok && expect(lastErrorContains("selected backend is 'mock_gpu'"),
                    "wrong backend free names selected backend");

  setenv("SHAKTI_BACKEND", "cpu", 1);
  ok = ok && expect(shaktiFree(ptr) == SHAKTI_SUCCESS,
                    "owner backend can still free after rejected free");
  ok = ok && expect(std::strcmp(shaktiGetLastErrorMessage(), "success") == 0,
                    "successful free clears last error detail");

  unsetenv("SHAKTI_BACKEND");

  std::cout << (ok ? "PASS" : "FAIL") << "\n";
  return ok ? 0 : 1;
}
