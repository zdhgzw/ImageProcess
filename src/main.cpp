#include "ImageProcessingApp.h"
#include <iostream>

int main() {
    try {
        ImageProcessingApp app;
        app.initialize();
        app.run();
    } catch (const std::exception& e) {
        std::cerr << "Application error: " << e.what() << std::endl;
        return -1;
    } catch (...) {
        std::cerr << "Unknown error" << std::endl;
        return -1;
    }
    
    return 0;
}