/*
 * DukeWidget.cpp
 *
 *  Created on: May 7, 2012
 *      Author: Guillaume Chatelet
 */

#include "DukeWidget.h"

#include "DukeEngine.h"

#include "utils/QtEventToProtobuf.h"
#include <dukeapi/ProtobufSerialize.h>

#include <cassert>

DukeWidget::DukeWidget(const QGLFormat& format, QWidget* parent, const QGLWidget* shareWidget, Qt::WindowFlags flags) :
                QGLWidget(format, parent, shareWidget, flags), m_pEngine(NULL), texturePool(renderer) {
}

DukeWidget::~DukeWidget() {
}

void DukeWidget::initializeGL() {
    renderer.initializeGL();
    connect(&m_Timer, SIGNAL(timeout()), this, SLOT(updateGL()));
    m_Timer.start();
}

void DukeWidget::resizeGL(int width, int height) {
    renderer.windowResized(width, height);
}

void DukeWidget::updateGL() {
    boost::mutex::scoped_lock lock(m_Mutex);
    if (m_pEngine)
        m_pEngine->onUpdate(*this);
}

using namespace ::duke::protocol;
using namespace google::protobuf::serialize;

bool DukeWidget::event(QEvent * pEvent) {
    assert(pEvent);

    Event event;
    {
        boost::mutex::scoped_lock lock(m_Mutex);
        if (m_pEngine && Update(event, *pEvent)) {
            MessageHolder holder;
            pack(event, holder);
            m_pEngine->onEvent(holder);
        }
    }

    return QGLWidget::event(pEvent);
}

void DukeWidget::bind(IDukeEngine *pEngine) {
    boost::mutex::scoped_lock lock(m_Mutex);
    m_pEngine = pEngine;
}
