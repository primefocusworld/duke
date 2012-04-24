/*
 * RenderInterface.cpp
 *
 *  Created on: 4 avr. 2012
 *      Author: Guillaume Chatelet
 */

#include "RenderInterface.h"
#include "renderer/RenderingEngine.h"
#include "renderer/ogl/OGLRenderer.h"

#include <SFML/Window/Window.hpp>

#include <iostream>

#define SFML_WINDOW_TITLE "Duke Player"

using namespace std;

IRendererHost::~IRendererHost() {
}

void createWindowAndLoop(duke::protocol::Renderer configuration, IRendererHost& host) {
    try {
        const sf::VideoMode desktopMode = sf::VideoMode::GetDesktopMode();
        unsigned long windowStyle = 0;
        if (configuration.fullscreen())
            configuration.set_decoratewindow(false);
        if (configuration.decoratewindow()) {
            windowStyle |= sf::Style::Resize;
            windowStyle |= sf::Style::Close;
        }
        if (configuration.fullscreen()) {
            /**
             * we don't want to set the fullscreen flag here
             * because it will trigger a WindowImplWin32::SwitchToFullscreen()
             * which will bring the refresh rate back to 60Hz
             * and prevent correct synchronization for playback
             */
            // windowStyle |= sf::Style::Fullscreen;
            configuration.set_width(desktopMode.Width);
            configuration.set_height(desktopMode.Height);
        }
        // creating the window
        sf::Window window;
        if (configuration.has_handle() && configuration.handle() != 0) {
#if defined(_WIN32) || defined(__WIN32__) || defined(__APPLE__)
            sf::ContextSettings settings;
            window.Create((HWND__*) configuration.handle(), settings);
#else
            window.Create( configuration.handle() );
#endif
        } else {
            window.Create(sf::VideoMode(configuration.width(), configuration.height(), desktopMode.BitsPerPixel), SFML_WINDOW_TITLE, windowStyle);
        }

        // initializing the renderer
        OGLRenderer ogl(configuration);
        RenderingEngine renderer(configuration, window, host, ogl);
        renderer.loop();
    } catch (exception& ex) {
        cerr << ex.what() << endl;
    }
}
