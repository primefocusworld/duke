#include "UIGraphicsTrack.h"
#include "UITracksView.h"

UIGraphicsTrack::UIGraphicsTrack(quint32 trackNumber, QGraphicsItem *parent) :
    QGraphicsWidget(parent) {
    m_trackNumber = trackNumber;
    m_enabled = true;
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setContentsMargins(0, 0, 0, 0);
    setZValue(1);
}

void UIGraphicsTrack::setHeight(int height) {
    setPreferredHeight(height);
    adjustSize();
    updateGeometry();
}

int UIGraphicsTrack::height() {
    return preferredHeight();
}

void UIGraphicsTrack::setTrackEnabled(bool enabled) {
    if (enabled == m_enabled)
        return;
    m_enabled = enabled;
}

bool UIGraphicsTrack::isEnabled() {
    return m_enabled;
}

quint32 UIGraphicsTrack::trackNumber() {
    return m_trackNumber;
}

