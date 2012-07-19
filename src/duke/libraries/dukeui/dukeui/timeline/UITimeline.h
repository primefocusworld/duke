#ifndef UITIMELINE_H
#define UITIMELINE_H

#include "ui_Timeline.h"
#include <set>

// forward declaration
class UITracksScene;
class UITracksView;
class UITracksRuler;

class UITimeline : public QDockWidget {

    Q_OBJECT
    Q_DISABLE_COPY(UITimeline)

public:
    UITimeline();
    virtual ~UITimeline(){}

    void showEvent ( QShowEvent * event );

public slots:
    void setDuration(int duration);

private:
    Ui::Timeline ui;
    UITracksView* mView;
    UITracksScene* mScene;
    UITracksRuler* mRuler;
};

#endif // UITIMELINE_H
