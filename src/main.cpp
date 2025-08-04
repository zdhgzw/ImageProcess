#include "ImageProcessingApp.h"
#include <iostream>

int main() {
    try {
        ImageProcessingApp app;
        app.initialize();
        app.run();
    } catch (const std::exception& e) {
        std::cerr << "程序运行错误: " << e.what() << std::endl;
        return -1;
    } catch (...) {
        std::cerr << "未知错误" << std::endl;
        return -1;
    }
    
    return 0;
}