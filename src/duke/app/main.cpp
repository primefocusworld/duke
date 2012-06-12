#include "Configuration.h"
#include <player.pb.h>
#include <dukeapi/messageBuilder/QuitBuilder.h>
#include <dukeapi/QueueMessageIO.h>
#include <dukeapi/SocketMessageIO.h>
#include <dukeengine/renderer/DukeEngine.h>
#include <dukeengine/renderer/DukeWidget.h>
#include <dukeui/MainWindow.h>
#include <QApplication>
#include <iostream>
#include <cstdlib>

using namespace std;

class DukeApplication : public QApplication {
public:
    DukeApplication(int &argc, char **argv, bool GUIenabled, int = QT_VERSION) :
                    QApplication(argc, argv, GUIenabled) {
    }

    virtual bool notify(QObject * receiver, QEvent * e) {
        try {
            return QApplication::notify(receiver, e);
        } catch (quit_exception &e) {
            const string message(e.what());
            if (!message.empty())
                cerr << "Exiting : " << message << endl;
            QApplication::exit(e.returnValue);
        } catch (exception &e) {
            cerr << "Abnormal termination : " << e.what() << endl;
            QApplication::exit();
        }
        return false;
    }
};

/////////////////
// CmdLine App //
/////////////////
int runDuke(int argc, char** argv, const Configuration& configuration, IMessageIO &io) {
    DukeApplication qapp(argc, argv, true);

    Qt::WindowFlags windowflags = 0;
    DukeWidget widget(QGLFormat(), 0, 0, windowflags);

    const duke::protocol::Renderer renderer = waitForRenderer(io);
    if (renderer.fullscreen())
        widget.showFullScreen();
    else
        widget.showNormal();

    DukeEngine engine(configuration.imageFactory(), configuration.cache(), io);
    widget.bind(&engine);

    return qapp.exec();
}


int runDukeX(int argc, char** argv, const Configuration& configuration, IMessageIO &io) {
    // Qt resources & CSS
    Q_INIT_RESOURCE(resources);

    DukeApplication qapp(argc, argv, true);

    QFile file(":/CSS/dark.qss");
    if (file.open(QIODevice::ReadOnly)) {
        QString styleSheet = QLatin1String(file.readAll());
        qapp.setStyleSheet(styleSheet);
    }

    Qt::WindowFlags windowflags = 0;
    MainWindow mw;
    mw.setUnifiedTitleAndToolBarOnMac(true);

    DukeWidget widget(QGLFormat(), &mw, 0, windowflags);
    mw.setCentralWidget(&widget);
    widget.setFocusPolicy(Qt::StrongFocus);
    widget.setFocus(Qt::OtherFocusReason);

    DukeEngine engine(configuration.imageFactory(), configuration.cache(), io);
    widget.bind(&engine);

    const duke::protocol::Renderer renderer = waitForRenderer(io);
    if (renderer.fullscreen())
        mw.showFullScreen();
    else
        mw.showNormal();

    mw.raise();

    return qapp.exec();
}

/////////////////
// Server mode //
/////////////////
struct SessionCreator {
    SessionCreator(QueueMessageIO& _io) :
                    io(_io) {
    }
    google::protobuf::serialize::ISession* create(boost::asio::io_service& service) {
        return new SocketSession(service, io.inputQueue, io.outputQueue);
    }
private:
    QueueMessageIO& io;
};

int runServer(int argc, char** argv, const Configuration& configuration) {
    using namespace boost::asio;
    using namespace boost::asio::ip;
    using google::protobuf::serialize::duke_server;
    int returnValue = EXIT_SUCCESS;
    do {
        QueueMessageIO io;
        tcp::endpoint endpoint(tcp::v4(), configuration.port());
        // -> c++0x version, cool but need a gcc version > 4.4
        //            auto sessionCreator = [&io](io_service &service) {return new SocketSession(service, io.inputQueue, io.outputQueue);};
        //            duke_server server(endpoint, sessionCreator);
        SessionCreator creator(io);
        duke_server server(endpoint, boost::bind(&SessionCreator::create, &creator, _1));
        boost::thread io_launcher(&duke_server::run, &server);
        returnValue = runDuke(argc, argv, configuration, io);
        io_launcher.join();
    } while (returnValue == EXIT_RELAUNCH);
    return returnValue;
}

//////////
// main //
//////////
int main(int argc, char** argv) {
    try {
        Configuration configuration(argc, argv);

        switch (configuration.mode()) {
            case Configuration::DUKE:
                return runDuke(argc, argv, configuration, configuration.io());
            case Configuration::DUKEX:
                return runDukeX(argc, argv, configuration, configuration.io());
            case Configuration::SERVER:
                return runServer(argc, argv, configuration);
            default:
            case Configuration::NO_OP:
                return EXIT_SUCCESS;
        }

    } catch (exception& e) {
        cerr << e.what() << endl;
    } catch (...) {
        cerr << "Unexpected error." << endl;
    }
    return EXIT_FAILURE;
}
