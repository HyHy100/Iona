#include <Iona.hpp>

#include <iostream>

int main()
{
	iona::Window window(iona::uSize_t(800, 800), "MIWA VULKAN EDITION");

    while (window.pollEvents()) {
        window.present();
    }

    return 0;
}