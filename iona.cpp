#include <Iona.hpp>

#include <iostream>

int main()
{
	iona::Window window(iona::SizeUint(800, 800), "MIWA VULKAN EDITION");
    
    while (window.pollEvents()) {
        window.present();
    }

    return 0;
}