#include "UITracksScene.h"
#include "UIGraphicsMovieItem.h"
#include "UIGraphicsTrack.h"
#include "UITimeline.h"
#include <QMenu>
#include <QGraphicsSceneContextMenuEvent>

UITracksScene::UITracksScene(QObject* parent) :
    QGraphicsScene(parent) {
}

void UITracksScene::keyPressEvent(QKeyEvent* keyEvent) {
    QGraphicsScene::keyPressEvent(keyEvent);
}

void UITracksScene::contextMenuEvent(QGraphicsSceneContextMenuEvent* event) {
    QGraphicsScene::contextMenuEvent(event);
    if (event->isAccepted())
        return; // Event handled by an item in the scene.
}
