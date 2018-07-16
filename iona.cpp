#include <Iona.hpp>

#include <thread>
#include <chrono>

#include <iostream>

int main()
{
	iona::Window window(iona::SizeUint(800, 800), "Engine V0.0.1 Vulkan");
    
    iona::Texture tex("aa.png");
 
    bool s = false;

    iona::StaticVertexList<4> vlist {{
        iona::Vertex(iona::Vector4f(0.f, 0.f, 0.f, 1.f)),
        iona::Vertex(iona::Vector4f(1.f, 0.f, 0.f, 1.f)),
        iona::Vertex(iona::Vector4f(1.f, 1.f, 0.f, 1.f)),
        iona::Vertex(iona::Vector4f(0.f, 1.f, 0.f, 1.f))
    }};

    iona::VertexBuffer vb(vlist);

    while (window.pollEvents()) {
        std::cout << "loop..." << std::endl;

        tex.bind();

        std::cout << "wwwa..." << std::endl;
        
        assert(iona::priv::VkEnv::commands.currentBuffer);
        
        iona::priv::VkEnv::commands.currentBuffer.bindVertexBuffers(
            0U,
            { 
                vb.getBuffer()
            }, 
            { 
                0U 
            }
        );

        iona::priv::VkEnv::commands.currentBuffer.draw(4U, 1U, 0U, 0U);
                
        window.present();

        std::cout << "loop2..." << std::endl;
    }

    return 0;
}