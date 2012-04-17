#ifndef BUFFER_H_
#define BUFFER_H_

#include "IBufferBase.h"
#include <boost/shared_ptr.hpp>
#include <cassert>

template<class T>
class Buffer {
public:
    Buffer(IBufferBase* pBuffer = NULL) :
                    m_pBufferBase(pBuffer) {
    }

    T* lock(unsigned long offset = 0, unsigned long size = 0, unsigned long flags = 0) const {
        return reinterpret_cast<T*>(m_pBufferBase->lock(offset * sizeof(T), size * sizeof(T), flags));
    }

    void unlock() const {
        m_pBufferBase->unlock();
    }

    void fill(const T* data, std::size_t count) const {
        assert(data != NULL);

        T* MyData = lock();
        std::copy(data, data + count, MyData);
        unlock();
    }

    void release() {
        m_pBufferBase.reset();
    }

    const IBufferBase* getBuffer() const {
        return m_pBufferBase.get();
    }

    unsigned long size() const {
        if (m_pBufferBase == NULL)
            return 0;
        return m_pBufferBase->size();
    }

private:
    boost::shared_ptr<IBufferBase> m_pBufferBase;
};

#endif /* BUFFER_H_ */
