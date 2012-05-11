#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include <player.pb.h>

#include <boost/utility.hpp>
#include <boost/program_options.hpp>

#include <memory>

//forward declaration
class IMessageIO;
class ImageDecoderFactory;
class ImageDecoderFactoryImpl;

/**
 * Class to handle command line arguments, configuration files
 * and configuration of the application.
 */
struct Configuration : boost::noncopyable {
    enum Mode {
        NO_OP, SERVER, DUKE, DUKEX
    };

    Configuration(int argcc, char** argvv);

    IMessageIO& io() const;
    ImageDecoderFactory& imageFactory() const;

    Mode mode() const{return m_Mode;}
    duke::protocol::Cache cache() const{return m_Cache;}
    short port() const {return m_Port;}

private:
    void displayVersion();
    void displayHelp();

    boost::program_options::options_description m_CmdLineOnly;
    boost::program_options::options_description m_Config;
    boost::program_options::options_description m_Display;
    boost::program_options::options_description m_Interactive;
    boost::program_options::options_description m_CmdlineOptionsGroup;
    boost::program_options::options_description m_ConfigFileOptions;
    boost::program_options::options_description m_HiddenOptions;
    boost::program_options::variables_map m_Vm;

    std::auto_ptr<IMessageIO> m_pIO;
    std::auto_ptr<ImageDecoderFactory> m_pImageFactory;
    Mode m_Mode;
    duke::protocol::Cache m_Cache;
    short m_Port;
};

#endif /* CONFIGURATION_H_ */
