#include <Iona.hpp>

#include <iostream>
#include <thread>
#include <chrono>

int main()
{
	iona::Window window(iona::SizeUint(800, 800), "MIWA VULKAN EDITION");
    
    bool s = false;

    while (window.pollEvents()) {
        window.begin();
        assert(iona::priv::VKInfo::currentCommandBuffer);
        iona::priv::VKInfo::currentCommandBuffer.draw(4U, 1U, 0U, 0U);
        window.end();
        window.present();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000U/60U));
    }

    return 0;
}