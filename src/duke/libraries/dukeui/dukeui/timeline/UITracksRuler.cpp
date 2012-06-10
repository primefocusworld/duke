#include "UITracksRuler.h"
#include "UITracksView.h"
#include <QStylePainter>
#include <QPaintEvent>
#include <QDebug>
#include <iostream>

#define MAXHEIGHT 14
#define LITTLEMARKHEIGHT 2
#define MIDDLEMARKHEIGHT 5
#define BIGMARKHEIGHT 10

UITracksRuler::UITracksRuler(UITracksView* tracksView, QWidget* parent) :
    QWidget(parent), m_tracksView(tracksView), m_fps(25.0), m_duration(0), m_pos(0), m_zoomFactor(1.), m_panFactor(0) {

    setMinimumHeight(MAXHEIGHT);
    setMaximumHeight(MAXHEIGHT);
}

void UITracksRuler::setDuration(int duration) {
    m_duration = duration;
    update();
}

void UITracksRuler::paintEvent(QPaintEvent* e) {
    QStylePainter painter(this);
    painter.setClipRect(e->rect());

    QColor color(78, 78, 78, 255);
    painter.setPen(color);
    painter.setBrush(QBrush(color));
    painter.setFont(QFont("Arial", 11));

    // draw active area
    const int end = (int) (m_duration * m_zoomFactor);
    //if (end - m_panFactor > 1)
    //    painter.fillRect(0, 0, end - m_panFactor, height(), QBrush(QColor(77, 82, 95, 50)));

    // draw cursor (frame #)
    QRectF rect;
    rect = painter.boundingRect(rect, Qt::AlignHCenter, QString::number(m_pos));
    QPointF textpos(m_pos*m_zoomFactor - m_panFactor - rect.width() / 2., MAXHEIGHT - (rect.height() /4.));
    painter.drawText(textpos, QString::number(m_pos));

    // draw marks
    int littlemarkevery = 10;
    int middlemarkevery = 50;
    int bigmarkevery = 100;
    for (int f = 0; f < end; f+=littlemarkevery){
        if(f % bigmarkevery <= 0)
            painter.drawLine(f*m_zoomFactor - m_panFactor, MAXHEIGHT-BIGMARKHEIGHT, f*m_zoomFactor - m_panFactor, MAXHEIGHT);
        else if(f % middlemarkevery == 0)
            painter.drawLine(f*m_zoomFactor - m_panFactor, MAXHEIGHT-MIDDLEMARKHEIGHT, f*m_zoomFactor - m_panFactor, MAXHEIGHT);
        else if(f % littlemarkevery == 0)
            painter.drawLine(f*m_zoomFactor - m_panFactor, MAXHEIGHT-LITTLEMARKHEIGHT, f*m_zoomFactor - m_panFactor, MAXHEIGHT);
    }

}

void UITracksRuler::mouseDoubleClickEvent(QMouseEvent* event) {
    update();
}

void UITracksRuler::mousePressEvent(QMouseEvent* event) {
    if (event->buttons() == Qt::LeftButton && event->modifiers() == Qt::NoModifier) {
        emit cursorMoved((event->x() + m_panFactor)/m_zoomFactor);
    }
    update();
}

void UITracksRuler::mouseMoveEvent(QMouseEvent* event) {
    if (event->buttons() == Qt::LeftButton && event->modifiers() == Qt::NoModifier) {
        emit cursorMoved((event->x() + m_panFactor)/m_zoomFactor);
    }
    update();
}

void UITracksRuler::frameChanged(qint64 frame) {
    qDebug() << "UITracksRuler::frameChanged";
    m_pos = frame;
    update();
}

void UITracksRuler::zoomChanged(double zoomfactor) {
    m_zoomFactor = zoomfactor;
    qDebug() << "UITracksRuler::zoomChanged " << zoomfactor;
    update();
}

void UITracksRuler::panChanged(int panfactor) {
    m_panFactor = panfactor;
    qDebug() << "UITracksRuler::panChanged " << panfactor;
    update();
}


/*
const int UITracksRuler::comboScale[] = { 1, 2, 5, 10, 25, 50, 125, 250, 500, 725, 1500, 3000, 6000, 12000 };

UITracksRuler::UITracksRuler(UITracksView* tracksView, QWidget* parent) :
    QWidget(parent), m_tracksView(tracksView), m_duration(0), m_offset(0), m_displayMode(RDM_FRAME) {
    m_fps = 25.0;
    m_factor = 1;
    m_scale = 3;
    m_littleMarkDistance = FRAME_SIZE;
    m_mediumMarkDistance = FRAME_SIZE * m_fps;
    m_bigMarkDistance = FRAME_SIZE * m_fps * 60;
    setMinimumHeight(MAXHEIGHT);
    setPixelPerMark(5);
    // Redraw the ruler when the cursor position change
    connect(tracksView->tracksCursor(), SIGNAL(cursorMoved(qint64)), this, SLOT(update()));
}

void UITracksRuler::setPixelPerMark(double rate) {
    int scale = comboScale[(int) rate];
    m_factor = 1.0 / (double) scale * FRAME_SIZE;
    m_scale = 1.0 / (double) scale;
    double fend = m_scale * m_littleMarkDistance;
    switch ((int) rate) {
        case 0:
            m_textSpacing = fend;
            break;
        case 1:
            m_textSpacing = fend * m_fps;
            break;
        case 2:
        case 3:
        case 4:
            m_textSpacing = fend * m_fps * 5;
            break;
        case 5:
        case 6:
            m_textSpacing = fend * m_fps * 10;
            break;
        case 7:
            m_textSpacing = fend * m_fps * 30;
            break;
        case 8:
        case 9:
        case 10:
            m_textSpacing = fend * m_fps * 60;
            break;
        case 11:
        case 12:
            m_textSpacing = fend * m_fps * 300;
            break;
        case 13:
            m_textSpacing = fend * m_fps * 600;
            break;
    }
    update();
}

void UITracksRuler::setDuration(int duration) {
    m_duration = duration;
    update();
}

void UITracksRuler::paintEvent(QPaintEvent* e) {
    QStylePainter painter(this);
    painter.setClipRect(e->rect());

    // Draw the background
    const int projectEnd = (int) (m_duration * m_factor);
    if (projectEnd - m_offset > 1)
        painter.fillRect(0, 0, projectEnd - m_offset, height(), QBrush(QColor(77, 82, 95)));

    double f, step;
    int offsetMax;
    int offsetMin;
    const int maxVal = (e->rect().right() + m_offset) / FRAME_SIZE + 1;
    offsetMax = maxVal * FRAME_SIZE;
    offsetMin = (int) ((e->rect().left() + m_offset) / m_textSpacing);
    offsetMin = (int) (offsetMin * m_textSpacing);

    // Draw text
    QPalette palette;
    painter.setPen(palette.dark().color());
    for (f = offsetMin; f < offsetMax; f += m_textSpacing) {
        QString time = getTimeCode((int) (f / m_factor + 0.5));
        painter.drawText((int) f - m_offset + 2, LABEL_SIZE + 1, time);
    }

    // Draw cache state
    drawCacheState(&painter);

    // Draw the marks
    offsetMin = (e->rect().left() + m_offset) / m_littleMarkDistance;
    offsetMin = offsetMin * m_littleMarkDistance;
    step = m_scale * m_littleMarkDistance;
    if (step > 5)
        for (f = offsetMin - m_offset; f < offsetMax - m_offset; f += step)
            painter.drawLine((int) f, LITTLE_MARK_X1, (int) f, LITTLE_MARK_X2);
    offsetMin = (e->rect().left() + m_offset) / m_mediumMarkDistance;
    offsetMin = offsetMin * m_mediumMarkDistance;
    step = m_scale * m_mediumMarkDistance;
    if (step > 5)
        for (f = offsetMin - m_offset; f < offsetMax - m_offset; f += step)
            painter.drawLine((int) f, MIDDLE_MARK_X1, (int) f, MIDDLE_MARK_X2);
    offsetMin = (e->rect().left() + m_offset) / m_bigMarkDistance;
    offsetMin = offsetMin * m_bigMarkDistance;
    step = m_scale * m_bigMarkDistance;
    if (step > 5)
        for (f = offsetMin - m_offset; f < offsetMax - m_offset; f += step)
            painter.drawLine((int) f, BIG_MARK_X1, (int) f, BIG_MARK_X2);

    // Draw the pointer
    int cursorPosInView = m_tracksView->tracksCursor()->cursorPos();
    int cursorPos = cursorPosInView * m_factor - offset();

    QColor color(82, 97, 122, 150);
    painter.setPen(color);
    painter.setBrush(QBrush(color));
    painter.drawRect(cursorPos - 10, 0, 20, MAXHEIGHT);
    if (m_scale < 1) {
        QRectF rect;
        rect = painter.boundingRect(rect, Qt::AlignHCenter, QString::number((qreal)cursorPosInView));
        painter.drawText(cursorPos - (rect.width() / 2.f), 9, QString::number((qreal)cursorPosInView));
    }
}

void UITracksRuler::mouseDoubleClickEvent(QMouseEvent* event) {
    m_displayMode = (m_displayMode == RDM_FRAME) ? RDM_TC : RDM_FRAME;
    update();
}

void UITracksRuler::mousePressEvent(QMouseEvent* event) {
    if (event->buttons() == Qt::LeftButton && event->modifiers() == Qt::NoModifier) {
        emit frameChanged(qMax((qreal) 0, (qreal)qRound((event->x() + offset()) / m_factor)));
    }
    update();
}

void UITracksRuler::mouseMoveEvent(QMouseEvent* event) {
    if (event->buttons() == Qt::LeftButton && event->modifiers() == Qt::NoModifier) {
        emit frameChanged(qMax((qreal) 0, (qreal)qRound((event->x() + offset()) / m_factor)));
    }
    update();
}

// slot
void UITracksRuler::moveRuler(int pos) {
    m_offset = pos;
    update();
}

// slot
void UITracksRuler::framerateChanged(double framerate) {
    m_fps = framerate;
    update();
}

// private
void UITracksRuler::drawCacheState(QStylePainter *painter) {
    double step = m_scale * m_littleMarkDistance; // frame length (px)
    double offsetMax = (width() + m_offset) ;
    painter->save();
    painter->setPen(QColor(77, 255, 95));
    for (double f = m_offset-step ; f < offsetMax+step ; f += step){
        size_t frame = qRound(f / m_factor);
        if((int)frame > m_duration)
            continue;
        std::set<size_t>::const_iterator it = mCacheState.find(frame);
        if(it != mCacheState.end()){
            int pxframe = qRound(frame)*m_factor;
            QLine l(pxframe-m_offset, height()-1, pxframe-m_offset+step, height()-1);
            painter->drawLine(l);
        }
    }
    painter->restore();
}

// private
QString UITracksRuler::getTimeCode(int frames) const {
    QString text;
    if (m_displayMode == RDM_FRAME) {
        text.append(QString::number(frames).rightJustified(2, '0', false));
    } else {
        int seconds = frames / m_fps;
        frames = frames % m_fps;
        int minutes = seconds / 60;
        seconds = seconds % 60;
        int hours = minutes / 60;
        minutes = minutes % 60;
        text.append(QString::number(hours).rightJustified(2, '0', false));
        text.append(':');
        text.append(QString::number(minutes).rightJustified(2, '0', false));
        text.append(':');
        text.append(QString::number(seconds).rightJustified(2, '0', false));
        text.append(':');
        text.append(QString::number(frames).rightJustified(2, '0', false));
    }
    return text;
}
*/
