#ifdef DUKE_OPEN_EXR
//
// EXR Format. This is a test reader to avoid using the readPixel function
// of OpenEXR. It might just replace OpenExrIO.cpp if it works better. 
// Atm we focus on only exr scanline zip compression line by line.
//
#include <vector>
#include <duke/imageio_plugins/openexr/MMappedExrStream.h>
#include <ImfHeader.h>

using namespace std;

class ExrFormat {

	// Format
	Imf::MMappedExrStream *m_pStream;
	const void *m_pBuffer;
	size_t m_BufferSize;
	Imf::Header m_Header;
	vector<Imath::Int64> m_LineOffsets;

public:
	ExrFormat(const void *pBuffer, size_t BufferSize) : 
		m_pStream(nullptr), m_pBuffer(pBuffer), m_BufferSize(BufferSize), m_LineOffsets(1080) {
		
		m_pStream = new Imf::MMappedExrStream(const_cast<void*>(m_pBuffer), m_BufferSize);

		// Read header
		int Version = 0;
		m_Header.readFrom(*m_pStream, Version);
			
		// Check the exr is in the correct format
		//if (isTiled(Version)) {
		//	printf("Error unsupported format\n");
		//	throw 
		//}
		const Imath::Box2i &dataWindow = m_Header.dataWindow();
		const size_t nLines = dataWindow.max.y - dataWindow.min.y + 1;
		const size_t nRows = dataWindow.max.x - dataWindow.min.x + 1;

		// Read line offsets
		m_LineOffsets.resize(nLines);
		for (auto &offset : m_LineOffsets) {
			Imf::Xdr::read <Imf::StreamIO> (*m_pStream, offset);
		}

		// MaxBytePerLine
		const size_t NChannels = 4;
		const size_t BytesPerChannel = 2;
		const size_t MaxBytePerLine = NChannels * BytesPerChannels * nRows;

		
	}
	
	~ExrFormat() {
		delete m_pStream;
	}

	// ReadPixels
	void readPixelsTo(char *PixelBuffer) {
		
		// Read each line in the buffer
		for(auto Offset : m_LineOffsets) {
			// Get the position in the original buffer
			// Get the position in the dest buffer
			// uncompress to PixelBuffer
		}
		
	}
	

};


#endif//DUKE_OPEN_EXR
