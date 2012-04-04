/*
 * RenderInterface.cpp
 *
 *  Created on: 4 avr. 2012
 *      Author: Guillaume Chatelet
 */

#include "RenderInterface.h"
#include "renderer/ogl/OGLRenderer.h"

#define SFML_WINDOW_TITLE "Duke Player"

using namespace std;

IRendererHost::~IRendererHost() {
}

void createWindowAndLoop(duke::protocol::Renderer renderer, IRendererHost& host) {
    try {
        const sf::VideoMode desktopMode = sf::VideoMode::GetDesktopMode();
        unsigned long windowStyle = 0;
        if (renderer.fullscreen())
            renderer.set_decoratewindow(false);
        if (renderer.decoratewindow()) {
            windowStyle |= sf::Style::Resize;
            windowStyle |= sf::Style::Close;
        }
        if (renderer.fullscreen()) {
            /**
             * we don't want to set the fullscreen flag here
             * because it will trigger a WindowImplWin32::SwitchToFullscreen()
             * which will bring the refresh rate back to 60Hz
             * and prevent correct synchronization for playback
             */
            // windowStyle |= sf::Style::Fullscreen;
            renderer.set_width(desktopMode.Width);
            renderer.set_height(desktopMode.Height);
        }
        // creating the window
        sf::Window window;
        if (renderer.has_handle() && renderer.handle() != 0) {
#if defined(_WIN32) || defined(__WIN32__) || defined(__APPLE__)
            sf::ContextSettings settings;
            window.Create((HWND__*) renderer.handle(), settings);
#else
            window.Create( renderer.handle() );
#endif
        } else {
            window.Create(sf::VideoMode(renderer.width(), renderer.height(), desktopMode.BitsPerPixel), SFML_WINDOW_TITLE, windowStyle);
        }

        // initializing the renderer
        std::auto_ptr<IRenderer> m_pRenderer(new OGLRenderer(renderer, window, host));

        // engaging the rendering loop
        m_pRenderer->loop();
    } catch (exception& ex) {
        cerr << ex.what() << endl;
    }
}
