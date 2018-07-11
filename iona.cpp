#include <Iona.hpp>

#include <thread>
#include <chrono>

int main()
{
	iona::Window window(iona::SizeUint(800, 800), "MIWA VULKAN EDITION");
    
    bool s = false;

    iona::StaticVertexList<4> vlist {{
        iona::Vertex(iona::Vector4f(0.f, 0.f, 0.f, 1.f)),
        iona::Vertex(iona::Vector4f(1.f, 0.f, 0.f, 1.f)),
        iona::Vertex(iona::Vector4f(1.f, 1.f, 0.f, 1.f)),
        iona::Vertex(iona::Vector4f(0.f, 1.f, 0.f, 1.f))
    }};

    iona::VertexBuffer vb(vlist);

    while (window.pollEvents()) {    
        window.begin();
        
        assert(iona::priv::VKInfo::currentCommandBuffer);
        
        iona::priv::VKInfo::currentCommandBuffer.bindVertexBuffers(
            0U,
            { 
                vb.getBuffer() 
            }, 
            { 
                0U 
            }
        );

        iona::priv::VKInfo::currentCommandBuffer.draw(4U, 1U, 0U, 0U);
        
        window.end();
        
        window.present();
        
        std::this_thread::sleep_for(std::chrono::milliseconds(1000U/60U));
    }

    return 0;
}