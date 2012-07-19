#ifndef QT_EVENT_TO_PROTOBUF_H_
#define QT_EVENT_TO_PROTOBUF_H_

#include <QEvent>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <player.pb.h>
#include <iostream>

inline ::duke::protocol::Event_Type Get(const QEvent::Type& eventType) {
    using namespace ::duke::protocol;

    switch (eventType) {
        case QEvent::Close:
            return Event::CLOSED;
        case QEvent::Resize:
            return Event::RESIZED;
        case QEvent::FocusOut:
            return Event::LOSTFOCUS;
        case QEvent::FocusIn:
            return Event::GAINEDFOCUS;
        case QEvent::KeyPress:
            return Event::KEYPRESSED;
        case QEvent::KeyRelease:
            return Event::KEYRELEASED;
        case QEvent::Wheel:
            return Event::MOUSEWHEELMOVED;
        case QEvent::MouseButtonPress:
            return Event::MOUSEBUTTONPRESSED;
        case QEvent::MouseButtonRelease:
            return Event::MOUSEBUTTONRELEASED;
        case QEvent::MouseMove:
            return Event::MOUSEMOVED;
        case QEvent::Enter:
            return Event::MOUSEENTERED;
        case QEvent::Leave:
            return Event::MOUSELEFT;
        default:
#ifdef DEBUG
            std::cerr << "Unknown event type " << std::dec << eventType << "\t0x" << std::hex << eventType << std::endl;
#endif
            return Event::UNKNOWN;
    }
}

inline ::duke::protocol::KeyEvent_KeyCode Get(const QKeyEvent & e) {
    using namespace ::duke::protocol;

    switch (e.key()) {
        case Qt::Key_A:
            return KeyEvent::A;
        case Qt::Key_B:
            return KeyEvent::B;
        case Qt::Key_C:
            return KeyEvent::C;
        case Qt::Key_D:
            return KeyEvent::D;
        case Qt::Key_E:
            return KeyEvent::E;
        case Qt::Key_F:
            return KeyEvent::F;
        case Qt::Key_G:
            return KeyEvent::G;
        case Qt::Key_H:
            return KeyEvent::H;
        case Qt::Key_I:
            return KeyEvent::I;
        case Qt::Key_J:
            return KeyEvent::J;
        case Qt::Key_K:
            return KeyEvent::K;
        case Qt::Key_L:
            return KeyEvent::L;
        case Qt::Key_M:
            return KeyEvent::M;
        case Qt::Key_N:
            return KeyEvent::N;
        case Qt::Key_O:
            return KeyEvent::O;
        case Qt::Key_P:
            return KeyEvent::P;
        case Qt::Key_Q:
            return KeyEvent::Q;
        case Qt::Key_R:
            return KeyEvent::R;
        case Qt::Key_S:
            return KeyEvent::S;
        case Qt::Key_T:
            return KeyEvent::T;
        case Qt::Key_U:
            return KeyEvent::U;
        case Qt::Key_V:
            return KeyEvent::V;
        case Qt::Key_W:
            return KeyEvent::W;
        case Qt::Key_X:
            return KeyEvent::X;
        case Qt::Key_Y:
            return KeyEvent::Y;
        case Qt::Key_Z:
            return KeyEvent::Z;
        case Qt::Key_0:
            return KeyEvent::Num0;
        case Qt::Key_1:
            return KeyEvent::Num1;
        case Qt::Key_2:
            return KeyEvent::Num2;
        case Qt::Key_3:
            return KeyEvent::Num3;
        case Qt::Key_4:
            return KeyEvent::Num4;
        case Qt::Key_5:
            return KeyEvent::Num5;
        case Qt::Key_6:
            return KeyEvent::Num6;
        case Qt::Key_7:
            return KeyEvent::Num7;
        case Qt::Key_8:
            return KeyEvent::Num8;
        case Qt::Key_9:
            return KeyEvent::Num9;
        case Qt::Key_Escape:
            return KeyEvent::Escape;
        case Qt::Key_Menu:
            return KeyEvent::Menu;
        case Qt::Key_BracketLeft:
            return KeyEvent::LBracket;
        case Qt::Key_BracketRight:
            return KeyEvent::RBracket;
        case Qt::Key_Semicolon:
            return KeyEvent::SemiColon;
        case Qt::Key_Comma:
            return KeyEvent::Comma;
        case Qt::Key_Period:
            return KeyEvent::Period;
        case Qt::Key_Apostrophe:
            return KeyEvent::Quote;
        case Qt::Key_Slash:
            return KeyEvent::Slash;
        case Qt::Key_Backslash:
            return KeyEvent::BackSlash;
        case Qt::Key_AsciiTilde:
            return KeyEvent::Tilde;
        case Qt::Key_Equal:
            return KeyEvent::Equal;
        case Qt::Key_Space:
            return KeyEvent::Space;
        case Qt::Key_Return:
            return KeyEvent::Return;
        case Qt::Key_Back:
            return KeyEvent::Back;
        case Qt::Key_Tab:
            return KeyEvent::Tab;
        case Qt::Key_PageUp:
            return KeyEvent::PageUp;
        case Qt::Key_PageDown:
            return KeyEvent::PageDown;
        case Qt::Key_End:
            return KeyEvent::End;
        case Qt::Key_Home:
            return KeyEvent::Home;
        case Qt::Key_Insert:
            return KeyEvent::Insert;
        case Qt::Key_Delete:
            return KeyEvent::Delete;
        case Qt::Key_Plus:
            return KeyEvent::Add;
        case Qt::Key_Minus:
            return KeyEvent::Subtract;
        case Qt::Key_Asterisk:
            return KeyEvent::Multiply;
        case Qt::Key_Left:
            return KeyEvent::Left;
        case Qt::Key_Right:
            return KeyEvent::Right;
        case Qt::Key_Up:
            return KeyEvent::Up;
        case Qt::Key_Down:
            return KeyEvent::Down;
        case Qt::Key_F1:
            return KeyEvent::F1;
        case Qt::Key_F2:
            return KeyEvent::F2;
        case Qt::Key_F3:
            return KeyEvent::F3;
        case Qt::Key_F4:
            return KeyEvent::F4;
        case Qt::Key_F5:
            return KeyEvent::F5;
        case Qt::Key_F6:
            return KeyEvent::F6;
        case Qt::Key_F7:
            return KeyEvent::F7;
        case Qt::Key_F8:
            return KeyEvent::F8;
        case Qt::Key_F9:
            return KeyEvent::F9;
        case Qt::Key_F10:
            return KeyEvent::F10;
        case Qt::Key_F11:
            return KeyEvent::F11;
        case Qt::Key_F12:
            return KeyEvent::F12;
        case Qt::Key_F13:
            return KeyEvent::F13;
        case Qt::Key_F14:
            return KeyEvent::F14;
        case Qt::Key_F15:
            return KeyEvent::F15;
        case Qt::Key_Pause:
            return KeyEvent::Pause;
        default:
#ifdef DEBUG
            std::cerr << "Unknown keycode " << std::dec << e.key() << "\t0x" << std::hex << e.key() << std::endl;
#endif
            return KeyEvent_KeyCode_Unknown;
    }
}

inline ::duke::protocol::MouseEvent_Button Get(const Qt::MouseButton& mouseButton) {
    using namespace ::duke::protocol;
    switch (mouseButton) {
        case Qt::LeftButton:
            return MouseEvent::LEFT;
        case Qt::RightButton:
            return MouseEvent::RIGHT;
        case Qt::MiddleButton:
            return MouseEvent::MIDDLE;
        case Qt::XButton1:
            return MouseEvent::XBUTTONA;
        case Qt::XButton2:
            return MouseEvent::XBUTTONB;
        default:
#ifdef DEBUG
            std::cerr << "Unknown mouse button " << mouseButton << std::endl;
#endif
            return MouseEvent_Button_UNKNOWN;
    }
}

inline bool Update(::duke::protocol::Event& toUpdate, QEvent &withEvent) {
    using namespace ::duke::protocol;
    // setting event type
    toUpdate.set_type(Get(withEvent.type()));
    switch (toUpdate.type()) {
        case Event::KEYPRESSED:
        case Event::KEYRELEASED: {
            const QKeyEvent &e = static_cast<const QKeyEvent&>(withEvent);
            const Qt::KeyboardModifiers modifiers = e.modifiers();
            KeyEvent* const keyEvent = toUpdate.mutable_keyevent();
            keyEvent->set_alt(modifiers & Qt::AltModifier);
            keyEvent->set_control(modifiers & Qt::ControlModifier);
            keyEvent->set_shift(modifiers & Qt::ShiftModifier);
            keyEvent->set_code(Get(e));
            break;
        }
        case Event::RESIZED: {
            const QResizeEvent &e = static_cast<const QResizeEvent&>(withEvent);
            ResizeEvent* const resizeEvent = toUpdate.mutable_resizeevent();
            resizeEvent->set_width(e.size().width());
            resizeEvent->set_height(e.size().height());
            break;
        }
        case Event::MOUSEMOVED:
        case Event::MOUSEBUTTONPRESSED:
        case Event::MOUSEBUTTONRELEASED: {
            const QMouseEvent &e = static_cast<const QMouseEvent&>(withEvent);
            MouseEvent* const mouseEvent = toUpdate.mutable_mouseevent();
            mouseEvent->set_button(Get(e.button()));
            mouseEvent->set_x(e.x());
            mouseEvent->set_y(e.y());
            break;
        }
        case Event::MOUSEWHEELMOVED: {
            const QWheelEvent &e = static_cast<const QWheelEvent&>(withEvent);
            MouseEvent* const mouseEvent = toUpdate.mutable_mouseevent();
            mouseEvent->set_deltawheel(e.delta());
            break;
        }
        case Event::UNKNOWN:
        default:
#ifdef DEBUG
            std::cerr << "Unknown event " << toUpdate.type() << std::endl;
#endif
            return false;
    }
    return true;
}

#endif /* QT_EVENT_TO_PROTOBUF_H_ */
