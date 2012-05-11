#ifndef INTERACTIVEDECODER_H_
#define INTERACTIVEDECODER_H_

#include <dukeapi/IMessageIO.h>
#include <dukeapi/MessageQueue.h>

class InteractiveMessageIO : public IMessageIO {
public:
    InteractiveMessageIO();
    virtual ~InteractiveMessageIO();

    virtual void push(const ::google::protobuf::serialize::SharedHolder& holder);
    virtual void waitPop(::google::protobuf::serialize::SharedHolder& holder);
    virtual bool tryPop(::google::protobuf::serialize::SharedHolder& holder);

    MessageQueue m_ToApplicationQueue;
private:
    bool m_bPlay;
    unsigned int m_iFitMode;
    std::stringstream m_ssSeek;
};

#endif /* INTERACTIVEDECODER_H_ */
