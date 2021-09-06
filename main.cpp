#include "source/Project.h"

static imSquared squared;

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
    void WebUpdate(void) 
    {
        squared.step();
    }
#endif


int main(void)
{
    squared.setup();
    
#if defined(PLATFORM_WEB)

    emscripten_set_main_loop(WebUpdate, 0, 1);
#else
    squared.run();
#endif

    squared.shutdown();

    return 0;
}








