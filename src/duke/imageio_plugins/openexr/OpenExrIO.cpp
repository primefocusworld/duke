#ifdef DUKE_OPEN_EXR

// Specialized OpenEXR reader.
// It reads directly from the memory


#include <duke/gl/GL.hpp>
#include <duke/imageio/DukeIO.hpp>        // for IIODescriptor::Capability, etc
#include <duke/attributes/Attributes.hpp> // for Attributes
#include <duke/imageio/PackedFrameDescription.hpp>
#include <duke/engine/ColorSpace.hpp>
#include <stddef.h>                       // for size_t
#include <stdint.h>                       // for int32_t
#include <string>                         // for string
#include <vector>                         // for vector

// Include OpenEXR
#include <ImfRgbaFile.h>
#include <ImfFrameBuffer.h>
#include <ImfStandardAttributes.h>
#include <Iex.h>

namespace Imf {


/// This class provides basic functionnality to 
/// read from a mmapped buffer within the OpenEXR library
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

namespace duke {

class OpenExrReader: public IImageReader {

	Imf::MMappedExrStream *m_pStream;
	Imf::RgbaInputFile *m_pReader;
	Imath::Box2i m_DataWindow;

public:
	OpenExrReader(const IIODescriptor *pDesc, const void *pData, const size_t dataSize) :
			IImageReader(pDesc), 
			m_pStream(nullptr), 
			m_pReader(nullptr), 
			m_DataWindow() {

		m_pStream = new Imf::MMappedExrStream(const_cast<void*>(pData), dataSize);
		m_pReader = new Imf::RgbaInputFile(*m_pStream);
		m_DataWindow = m_pReader->dataWindow();

		// Description
		m_Description.height = m_DataWindow.max.y - m_DataWindow.min.y + 1;
		m_Description.width = m_DataWindow.max.x - m_DataWindow.min.x + 1;
		m_Description.glPackFormat = GL_RGBA16F;

		const size_t nchannels = 4;
		const size_t sizeofformat = 2;
		m_Description.dataSize = m_Description.height * m_Description.width * nchannels * sizeofformat;

		// Metadata
		m_Attributes.emplace_back("colorspace", (int) ColorSpace::Linear);
	}

	virtual ~OpenExrReader() {
		delete m_pReader;
		delete m_pStream;
	}

	virtual void readImageDataTo(void* pData) {
		if (!m_pReader) 
			return;
		m_pReader->setFrameBuffer (static_cast<Imf::Rgba*>(pData), 1, m_Description.width);

		// Note cyril : readPixels is taking too long, 
		// this function doesn't seem to be very efficient in the case of reading multiple files
		// at the same time
		m_pReader->readPixels (m_DataWindow.min.y, m_DataWindow.max.y);
	}
};

class OpenExrDescriptor: public IIODescriptor {

	// We want to have the ability to read from memory
	virtual bool supports(Capability capability) const {
		return capability == Capability::READER_READ_FROM_MEMORY;
	}
	
	virtual const std::vector<std::string>& getSupportedExtensions() const {
		static std::vector<std::string> extensions = { "exr" };
		return extensions;
	}
	virtual const char* getName() const {
		return "OpenExr";
	}
	virtual IImageReader* getReaderFromMemory(const void *pData, const size_t dataSize) const {
		return new OpenExrReader(this, pData, dataSize);
	}
};

namespace {
bool registrar = IODescriptors::instance().registerDescriptor(new OpenExrDescriptor());
}  // namespace

}  // namespace duke

#endif // DUKE_OPEN_EXR
