/*
 * DukeWidget.h
 *
 *  Created on: May 7, 2012
 *      Author: Guillaume Chatelet
 */

#ifndef DUKEWIDGET_H_
#define DUKEWIDGET_H_

#include "ogl/OGLRenderer.h"

#include <QGLWidget>
#include <QTimer>
#include <boost/thread/mutex.hpp>

class IDukeEngine;

class DukeWidget : public QGLWidget {
private:
    Q_OBJECT

    IDukeEngine *m_pEngine;
    friend class RenderHelper;
    OGLRenderer renderer;
    TexturePool texturePool;

public:
    DukeWidget(const QGLFormat& format, QWidget* parent = 0, const QGLWidget* shareWidget = 0, Qt::WindowFlags f = 0);
    ~DukeWidget();

    void bind(IDukeEngine *pEngine);
protected:
    virtual void initializeGL();
    virtual void resizeGL(int width, int height);
    virtual bool event(QEvent *);
protected slots:
    virtual void updateGL();
private:
    QTimer m_Timer;
    boost::mutex m_Mutex;
};

#endif /* DUKEWIDGET_H_ */
