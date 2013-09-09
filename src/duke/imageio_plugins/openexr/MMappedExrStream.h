#ifndef MMAPPEDEXRSTREAM_HPP
#define MMAPPEDEXRSTREAM_HPP

// Include OpenEXR
#include <ImfRgbaFile.h>
#include <ImfFrameBuffer.h>
#include <ImfStandardAttributes.h>
#include <Iex.h>

namespace Imf {


/// This class provides basic functionnality to 
/// read from a memory mapped buffer with the OpenEXR library
class MMappedExrStream : public IStream {
public:	
	MMappedExrStream(void *pBuffer, size_t BufferSize) : 
		IStream(""), m_pBuffer(pBuffer), m_BufferSize(BufferSize), m_ReadPosition(0) {
	}
	virtual ~MMappedExrStream(){};

	virtual bool isMemoryMapped() const {
		return true;
	}

	virtual char* readMemoryMapped(int n) {
		if (m_ReadPosition >= m_BufferSize)
			throw Iex::InputExc("Unexpected end of file");
		if (m_ReadPosition+n > m_BufferSize)
			throw Iex::InputExc("Reading past end of file");

		char *data = static_cast<char *>(m_pBuffer) + m_ReadPosition;
		m_ReadPosition += n;
		return data;
	}

	virtual bool read(char *pBuffer, int n) {
		if (m_ReadPosition >= m_BufferSize)
			throw Iex::InputExc("Unexpected end of file");
		if (m_ReadPosition+n > m_BufferSize)
			throw Iex::InputExc("Reading past end of file");
		memcpy(pBuffer, static_cast<char*>(m_pBuffer) + m_ReadPosition, n);
		m_ReadPosition += n;
		return m_ReadPosition < m_BufferSize;
	}
	virtual Int64 tellg() {
		return m_ReadPosition;
	}

	virtual void seekg(Int64 pos) {
		m_ReadPosition = pos;
	}

private:
	void *m_pBuffer;
	Int64 m_BufferSize;
	Int64 m_ReadPosition;
};
};//namespace Imf




#endif//MMAPPEDEXRSTREAM_HPP
