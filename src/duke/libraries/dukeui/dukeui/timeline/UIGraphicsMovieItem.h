#ifndef UIGRAPHICSMOVIEITEM_H
#define UIGRAPHICSMOVIEITEM_H

#include <QGraphicsItem>

class UIGraphicsMovieItem : public QObject, public QGraphicsItem {

    Q_OBJECT
#if QT_VERSION >= 0x040600
    Q_INTERFACES(QGraphicsItem)
#endif

public:
    enum {
        Type = UserType + 1
    };

public:
    UIGraphicsMovieItem(const QString& path, qint64 recin, qint64 recout, qint64 srcin, qint64 srcout);
    virtual ~UIGraphicsMovieItem() {
    }

public:
    virtual int type() const {
        return Type;
    }
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);
    virtual QRectF boundingRect () const;

protected:
    void paintRect(QPainter* painter, const QStyleOptionGraphicsItem* option);
    void paintTitle(QPainter* painter, const QStyleOptionGraphicsItem* option);

private:
    qint64 mWidth;
};

#endif // UIGRAPHICSMOVIEITEM_H
