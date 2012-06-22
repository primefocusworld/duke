#ifndef UIMESSAGEIO_H_
#define UIMESSAGEIO_H_

#include <dukeapi/IMessageIO.h>
#include <dukeapi/MessageQueue.h>

class UIMessageIO : public IMessageIO {
public:
    UIMessageIO();
    virtual ~UIMessageIO();

    virtual void push(const ::google::protobuf::serialize::SharedHolder& holder);
    virtual void waitPop(::google::protobuf::serialize::SharedHolder& holder);
    virtual bool tryPop(::google::protobuf::serialize::SharedHolder& holder);

    MessageQueue m_ToApplicationQueue;
private:
    bool m_bPlay;
    unsigned int m_iFitMode;
    std::stringstream m_ssSeek;
};

#endif // UIMESSAGEIO_H_
