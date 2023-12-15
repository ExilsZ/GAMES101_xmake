/***
 * @Author       : ExilsZ
 * @LastEditor   : ExilsZ
 * @Date         : 21-12-15 18:07
 * @LastEditTime : 23-09-16 17:40
 * @Description  : 测试中文字符文件名和输出和 C++ 版本；
 */

#include <iostream>

auto main() -> int {
    char a{0};
    std::cout << "Hello World!\n";

#if __cplusplus > 202302L
    std::cout << "你的编译器支持 C++23 以上\n";
#elif __cplusplus == 202302L
    std::cout << "你的编译器支持 C++23\n";
#elif __cplusplus == 202101L
    std::cout << "你的编译器支持 C++2a\n";
#elif __cplusplus == 202002L
    std::cout << "你的编译器支持 C++20\n";
#elif __cplusplus == 201703L
    std::cout << "你的编译器支持 C++17\n";
#elif __cplusplus == 201402L
    std::cout << "你的编译器支持 C++14\n";
#elif __cplusplus == 201103L
    std::cout << "你的编译器支持 C++11\n";
#else
    std::cout << "你的编译器支持 Pre-C++11\n";
#endif

    std::cout << "输入任意字符回车结束程序\n";
    std::cin >> a;
}
