/*
 * RenderInterface.cpp
 *
 *  Created on: 4 avr. 2012
 *      Author: Guillaume Chatelet
 */

#include "RenderInterface.h"

#include "renderer/RenderingEngine.h"

#include <QApplication>

#include <iostream>

using namespace std;

IRendererHost::~IRendererHost() {
}

void createWindowAndLoop(duke::protocol::Renderer configuration, IRendererHost& host) {
    try {
        int arguments = 1;
        const char * const filename = "duke";
        QApplication qapp(arguments, const_cast<char**>(&filename), true);

        Qt::WindowFlags windowflags = 0;
        RenderingEngine renderer(host, QGLFormat(), 0, 0, windowflags);

        if (configuration.fullscreen())
            renderer.showFullScreen();
        else
            renderer.showNormal();

        qapp.exec();
    } catch (exception& ex) {
        cerr << ex.what() << endl;
    }
}
