#ifndef UIGRAPHICSTRACK_H
#define UIGRAPHICSTRACK_H

#include <QGraphicsWidget>
#include <QList>

// forward declaration
class UIAbstractGraphicsItem;
class UIAbstractGraphicsMediaItem;

class UIGraphicsTrack : public QGraphicsWidget {

    Q_OBJECT

public:
    enum {
        Type = UserType + 2
    };

public:
    UIGraphicsTrack(quint32 trackNumber, QGraphicsItem *parent = 0);

public:
    int type() const {
        return Type;
    }
    void setHeight(int height);
    int height();
    void setTrackEnabled(bool enabled);
    bool isEnabled();
    quint32 trackNumber();

private:
    quint32 m_trackNumber;
    bool m_enabled;
};

#endif // UIGRAPHICSTRACK_H
