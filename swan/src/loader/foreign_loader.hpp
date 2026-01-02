// #pragma once
//
// #include "../utils/common.hpp"
//
// #ifdef OS_WINDOWS
// #    include <windows.h>
// #endif
//
// namespace spade
// {
//     class Library {
//         void *lib;
//
//       public:
//         Library(void *lib) : lib(lib) {}
//
//         template<typename ReturnType, typename... Args>
//         SwanResult<ReturnType> call(const string &function_name, Args... args) {
// #ifdef OS_WINDOWS
//             using FunctionType = ReturnType(CALLBACK *)(Args...);
//             FunctionType function = (FunctionType) GetProcAddress(lib, function_name.c_str());
//
//             if (function == null) {
//                 DWORD error_code = GetLastError();
//                 string err_msg = get_error_message(error_code, lib);
//                 return Error(SwanError(ErrorKind::NATIVE_LIBRARY, std::format("error code {}: {}", error_code, err_msg)));
//             }
//             return function(args...);
// #endif
// #ifdef OS_LINUX
//             // TODO: Implement for linux
// #endif
//         }
//     };
// }    // namespace spade
