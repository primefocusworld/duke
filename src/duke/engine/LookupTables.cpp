#include <fstream>
#include <sstream>
#include <vector>
#include <cstdio> // sscanf
#include "duke/engine/LookupTables.hpp"

namespace duke {

// anonymous functions
namespace {

//
// Should micmic the OpenGL 3D buffer packing
// 
struct Buffer3D {

	typedef float ComponentType;// GL_UNSIGNED_BYTE or GL_FLOAT ?

	ComponentType *rawBuffer; 
	static const unsigned int nbChannel=3; // GL_RGB
	unsigned int nbSamplePerChannel; // Resolved at creation, generally 16 or 32

	Buffer3D(unsigned int size) :
			rawBuffer(new ComponentType[nbChannel*size*size*size]), nbSamplePerChannel(size) {
		applyIdentity();// Not really needed, but cleaner 
	} 

	~Buffer3D() {
		if (rawBuffer) 
			delete [] rawBuffer;
	}

	// Sets the identity
	void applyIdentity() {
	}
	
	// apply a function to all elements 
	void applyMap() {
	
	}


	void uploadTo(gl::GlTexture3D &tex) {
		// Using texture unit 1 ?? check if the fonts are not using this one
		glActiveTexture(GL_TEXTURE1);
		//glEnable(GL_TEXTURE_3D);
		auto textureBound = tex.scope_bind_texture();
		// Interpolation ?
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Initialize the bounds of the 3d texture
		glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB, 
					 nbSamplePerChannel, nbSamplePerChannel, nbSamplePerChannel, 
					 0, GL_RGB, GL_FLOAT, rawBuffer);
	}

};

/// Decode cube file 
Buffer3D *decodeCube(const char *filename) {
	
    std::ifstream infile(filename);
	if (!infile.is_open())
		return nullptr;

    std::string line;
    std::vector<int>::iterator tableIt;

	enum class FileParts : char {Header, LutData};
	//readHeader, readContent
	Buffer3D *buffer = nullptr;
	Buffer3D::ComponentType *bufIt = nullptr;
	Buffer3D::ComponentType r, g, b;
	FileParts part = FileParts::Header;
	int nbSteps = 0; 
    while(std::getline(infile, line)) {
        // Pass comments and new lines
		if (line[0] == '\n')
			continue;
		if (line[0] == '#')
			continue;
		if (line[0] == 'T') // Assuming TITLE keyword
			continue;
		// Read the size 
		if (part == FileParts::Header) {
			if (sscanf(line.c_str(), "LUT_3D_SIZE %d", &nbSteps) == 1) {
				buffer = new Buffer3D(nbSteps);
				if (buffer) {
					bufIt = buffer->rawBuffer;
					// Jump to new part of the file
					part = FileParts::LutData;
					continue;
				}// else throw ?	
			}

		} else if (part == FileParts::LutData) {
			std::stringstream sline(line);
			sline >> r >> g >> b;
			*bufIt++ = r;
			*bufIt++ = g;
			*bufIt++ = b;
		}
	}

	return buffer;
}


};// anonymous namespace


LookupTransform::LookupTransform() :
	lookup3d() {
}


// Readers for different formats

// OpenGL code

LookupTransform* createFromFile(const char *) {
	// TODO : read lookup code
	//
	//
	// Look for extension, 3dl, cub, etc...

	// 3 parts in the file 


	return nullptr;
}
//

LookupTransform * defaultsRGB() {

	//const unsigned int s=16;
	//const unsigned int sss=s*s*s;
	//buffer.applyMap(srgbFunction);
	//auto transform = new LookupTransform();
	
//	Buffer3D buffer(s);
//	float *bufIt = buffer.rawBuffer;
//	for (unsigned int i=0; i<s; i++)
//		for (unsigned int j=0; j<s; j++)
//			for (unsigned int k=0; k<s; k++) {
//				*bufIt++ = static_cast<float>(k)/static_cast<float>(s-1);
//				*bufIt++ = static_cast<float>(j)/static_cast<float>(s-1);
//				*bufIt++ = static_cast<float>(i)/static_cast<float>(s-1);
//			}

	Buffer3D *buffer = decodeCube("/tmp/mytestlut.cube");

	auto transform = new LookupTransform();
	buffer->uploadTo(transform->lookup3d);
	delete buffer;

	return transform;
}

// glsl code 
const char *pLookupTransformFunc =
		R"(
uniform sampler3D lookup3d;
//uniform sampler1D lookup1d;
	
vec4 apply3dTransform(vec4 pix) {
	// TODO add 3x1d lookup
	// FIXME : depends on size of the texture !!!
	return texture(lookup3d, clamp(pix.rgb, 0, 1)*(15.0/16.0)+1.0/32.0);
} 

)";

};//namespace duke
