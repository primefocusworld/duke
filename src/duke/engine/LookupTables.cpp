#include <fstream>
#include <sstream>
#include <vector>
#include <cstdio> // sscanf
#include <cmath>

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
		// NOTE that this might go in scope_bind_texture ...
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

//
// Should micmic the OpenGL 1D buffer packing
//
struct Buffer1D {

    typedef float ComponentType;// GL_UNSIGNED_BYTE or GL_FLOAT ?

    ComponentType *rawBuffer;
    unsigned int nbSamplePerChannel; // Resolved at creation, generally 16 or 32

    Buffer1D(unsigned int size) :
           rawBuffer(new ComponentType[size]), nbSamplePerChannel(size)
    {
        applyIdentity();// Not really needed, but cleaner
    }

    ~Buffer1D() {
        if (rawBuffer)
            delete [] rawBuffer;
    }

    // Sets the identity
    void applyIdentity() {
    }

    // apply a function to all elements
    void applyMap() {

    }

    void uploadTo(gl::GlTexture1D &tex) {
        // Using texture unit 1 ?? check if the fonts are not using this one
        // NOTE that this might go in scope_bind_texture ...
        glActiveTexture(GL_TEXTURE2);
        //glEnable(GL_TEXTURE_3D);
        auto textureBound = tex.scope_bind_texture();
        // Interpolation ?
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Initialize the bounds of the 3d texture
        glTexImage1D(GL_TEXTURE_1D, 0, GL_RED,
                     nbSamplePerChannel,
                     0, GL_RED, GL_FLOAT, rawBuffer);
    }

};



/// Decode cube file 
LookupTransform* decodeCube(const char *filename) {
	
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

    auto transform = new LookupTransform();
    transform->lut3DSize = nbSteps;
    buffer->uploadTo(transform->lookup3d);
    delete buffer;

    return transform;
}

/// Decode 3dl file
LookupTransform* decode3dl(const char *filename) {

    std::ifstream infile(filename);
	if (!infile.is_open())
		return nullptr;

    std::string line;

	enum class FileParts : char {Header, LutData, Footer};
	//readHeader, readContent
    Buffer3D *buffer = nullptr;
	Buffer3D::ComponentType r, g, b;
	FileParts part = FileParts::Header;
    int nbSteps = 0, colorMax = 0;
	float invColorMax = 0;
	int cpt =0; // A compteur to know when you quit the LutData Part.

    while(std::getline(infile, line)) {
	    // Pass comments and new lines
		if (line[0] == '\n')
			continue;
		if (line[0] == '#') // Assuming that is a comment
			continue;

		if (part == FileParts::Header) 
		{
			if (sscanf(line.c_str(), "Mesh %d %d", &nbSteps, &colorMax) == 2) 
			{
					nbSteps = ( 1 << nbSteps ) + 1;
					colorMax = 1 << colorMax ;
                    invColorMax = float( 1. / colorMax );
					buffer = new Buffer3D(nbSteps);
					// HACK : Bypass next line (it is a scale of values)
					std::getline(infile, line);
					if (buffer) {
						// Jump to new part of the file
						part = FileParts::LutData;
						continue;
					}// else throw ?	
            }
		} else if (part == FileParts::LutData) {
			std::stringstream sline(line);
			sline >> r >> g >> b;

            // Compute the index
            int x = cpt % nbSteps,
                y = (cpt / nbSteps) % nbSteps,
                z = (( cpt / nbSteps) / nbSteps) % nbSteps;

            int idx =  z  + y * nbSteps + x * nbSteps *nbSteps;

            buffer->rawBuffer[3*idx]   = r * invColorMax;
            buffer->rawBuffer[3*idx+1] = g * invColorMax;
            buffer->rawBuffer[3*idx+2] = b * invColorMax;

            if(++cpt >= nbSteps*nbSteps*nbSteps)
				part = FileParts::Footer;
			
		} else if (part == FileParts::Footer)
			break ; // TODO	
	}

    auto transform = new LookupTransform();
    transform->lut3DSize = nbSteps;
    buffer->uploadTo(transform->lookup3d);
    delete buffer;

    return transform;
}

LookupTransform* decodeCsp(const char *filename)
{
    std::ifstream infile(filename);
    if (!infile.is_open())
        return nullptr;

    std::string line;

    enum class FileParts : char {Header, Lut1DData, Lut3DData};
    //readHeader, readContent
    Buffer3D *buffer = nullptr;
    Buffer3D::ComponentType r, g, b;
    //LUT
    Buffer1D *lut = nullptr;
    Buffer1D::ComponentType tmp, bmin, bmax;

    FileParts part = FileParts::Header;
    int nbSteps = 0;
    int lut1DSize = -1, cpt = 0, cptLut =0;
    // Skip the 1D Luts

    while(std::getline(infile, line)) {
        // Pass comments and new lines
        if (line[0] == '\n')
            continue;
        if (line[0] == '#')
            continue;
        if ( line.compare("CSPLUTV100") == 0 || line.compare("3D") == 0 )
            continue;
        // Read the size
        if (part == FileParts::Header) {

            if (sscanf(line.c_str(), "%d", &lut1DSize) == 1)
            {
                if(!lut)
                {
                    lut = new Buffer1D(lut1DSize);
                }

                // Jump the first line which is the indexes
                std::getline(infile, line);
                std::stringstream sline(line);
                sline >> bmin;
                float mark = bmin;

                for(int i=1; i<lut1DSize-1; i++)
                {
                    sline >> tmp;
                    if( fabs(mark-bmin) > 1./float(lut1DSize) * .1 )
                         throw std::runtime_error("Not Regular LUT 1D - Not yet implemented");
                }
                sline >> bmax;

                std::getline(infile, line);

                sline.str("");
                sline.clear();

                sline << line;


                for(int i=0; i<lut1DSize; i++)
                {
                    sline >> tmp;
                    if(cptLut > 0 )
                        if(tmp != lut->rawBuffer[i])
                               throw std::runtime_error("LUT different for R G and B - Not yet implemented");
                    lut->rawBuffer[i] =  tmp;
                }
                cptLut++;
            }
            if(cptLut > 2)
            {
                    part = FileParts::Lut3DData;
                    continue;
            }
        }
        else if (part == FileParts::Lut3DData)
        {
            int tmp1, tmp2, tmp3;
            if (sscanf(line.c_str(), "%d %d %d", &tmp1, &tmp2, &tmp3) == 3)
            {
                nbSteps = tmp1;
               // Initialization of the buffer
                buffer = new Buffer3D(nbSteps);
            }
            else if ( sscanf(line.c_str(), "%f %f %f", &r, &g, &b) == 3)
            {
                assert(nbSteps);

                // Compute the index
                int x = cpt % nbSteps,
                    y = (cpt / nbSteps) % nbSteps,
                    z = (( cpt / nbSteps) / nbSteps) % nbSteps;

                int idx =  z* nbSteps *nbSteps  + y * nbSteps + x ;

                buffer->rawBuffer[3*idx]   = r ;
                buffer->rawBuffer[3*idx+1] = g ;
                buffer->rawBuffer[3*idx+2] = b ;

                cpt++;

            }
        }
    }

    auto transform = new LookupTransform();
    transform->lut3DSize = nbSteps;
    buffer->uploadTo(transform->lookup3d);

    transform->lut1DSize = lut1DSize ;
    transform->lookup1d_max = bmax ;
    transform->lookup1d_min = bmin ;
    lut->uploadTo(transform->lookup1d);



    delete lut;
    delete buffer;

    return transform;


}



};// anonymous namespace


LookupTransform::LookupTransform() :
    lookup1d(),
    lookup3d(),
    lut1DSize(0),
    lut3DSize(0),
    lookup1d_max(0.),
    lookup1d_min(0.)
{
}


// Load an identity Lut

LookupTransform* createIDLut(uint size) {
	// Create the buffer
	Buffer3D *buffer = new Buffer3D(size);
	Buffer3D::ComponentType *bufIt = buffer->rawBuffer;

	// Precompute the inverse of the size
	float invSizef = float(1. / ( size - 1. ));
	
	// Pull the colors on the LUT - RGB
        for(uint i = 0; i < size * size * size; i++)
        {
            *bufIt++ = (float)(i%size) * invSizef;
            *bufIt++ = (float)((i/size)%size) * invSizef;
            *bufIt++ = (float)((i/size/size)%size) * invSizef;
        }	

	auto transform = new LookupTransform();
    transform->lut3DSize = size;
    buffer->uploadTo(transform->lookup3d);
	delete buffer;


	return transform;
}
//


// Readers for different formats

// OpenGL code

LookupTransform* createFromFile(const std::string &lutFilePath) {
	// TODO : read lookup code
	//
	//
	// Look for extension, 3dl, cub, etc...

	// 3 parts in the file 
	//

    int size = lutFilePath.length();
    int marker = -1;
    for(int idx = size - 1; idx > 0; idx--)
    {
        if(lutFilePath.c_str()[idx] == '.')
        {
            marker = idx+1;
            break;
        }

    }

    std::string  ext = lutFilePath.substr(marker, size-1);

    if(ext.compare("cube") == 0)
    {
        return decodeCube(lutFilePath.c_str());
    }
    else if(ext.compare("3dl") == 0)
    {
        return decode3dl(lutFilePath.c_str());
    }
    else if(ext.compare("csp") == 0)
    {
        return decodeCsp(lutFilePath.c_str());
    }
    else
    {
        printf("Duke does know the file format of your LUT, please use 3dl or cube");
    }

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

    return decodeCube("/tmp/mytestlut.cube");

//	auto transform = new LookupTransform();
//    transform->lut3DSize = buffer->nbSamplePerChannel;
//	buffer->uploadTo(transform->lookup3d);
//	delete buffer;

//	return transform;
}

// glsl code 
const char *pLookupTransformFunc =
		R"(
uniform sampler3D lookup3d;
uniform float lutSize;

uniform sampler1D lookup1d;
uniform float lookup1d_min, lookup1d_max, lookup1d_size ;


vec4 apply3dTransform(vec4 pix) {
	// TODO add 3x1d lookup
	// FIXME : depends on size of the texture !!!

    if( lookup1d_min != lookup1d_max )
    {
        float scale_1d =  ( lookup1d_size - 1. )/ lookup1d_size;
        float offset_1d = 1. / (2. * lookup1d_size);

        pix = (pix - lookup1d_min) / (lookup1d_max - lookup1d_min  ) ;

        pix.r = texture(lookup1d,clamp(pix.r,0.,1.) * scale_1d + offset_1d);
        pix.g = texture(lookup1d,clamp(pix.g,0.,1.) * scale_1d + offset_1d);
        pix.b = texture(lookup1d,clamp(pix.b,0.,1.) * scale_1d + offset_1d);
    }

    float scale =  ( lutSize - 1. )/ lutSize;
    float offset = 1. / (2. * lutSize);

    return texture(lookup3d, clamp(pix.rgb,0.,1.) * scale + offset );
} 

)";

};//namespace duke
