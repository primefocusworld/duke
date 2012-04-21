#ifndef TEXTUREPOOL_H_
#define TEXTUREPOOL_H_

#include <dukeio/Formats.h>

struct PoolRequest
{
    struct Dimension {
        int x,y,z;
        Dimension(int x,int y,int z) : x(x),y(y),z(z){}
    };
	const TPixelFormat format;
	const bool mipmap;
	const Dimension dimension;
	PoolRequest( const TPixelFormat& format, const bool mipmap, const int width, const int height, const int depth = 0 )
		: format( format ),
		mipmap( mipmap ),
		dimension( width, height, depth ) {}

};

#include "ResourceCache.h"

#include <functional>

struct PoolRequestLess : public std::binary_function<PoolRequest, PoolRequest, bool>
{
	bool operator()( const PoolRequest& first, const PoolRequest& second ) const
	{
		return first.format < second.format ? true : ( first.format == second.format && !first.mipmap && second.mipmap );
	}

};

class ScopedTexture;

#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>
#include <map>
#include <list>
class TexturePool : public boost::noncopyable
{
public:
    typedef ::resource::SharedResourcePtr ResourcePtr;
private:
	struct TextureHolder
	{
		const PoolRequest m_Request;
		ResourcePtr m_pResource;
		TextureHolder( const PoolRequest&, const ResourcePtr& );
	};
	typedef ::boost::shared_ptr<TextureHolder> TextureHolderPtr;

public:
	TexturePool();
	~TexturePool();

	typedef ::boost::shared_ptr<ScopedTexture> ScopedTexturePtr;
	void             put( const PoolRequest& request, const ResourcePtr& pResource );
	ScopedTexturePtr putAndGet( const PoolRequest& request, const ResourcePtr& pResource );
	ScopedTexturePtr get( const PoolRequest& request );

	static int computeDistance( const PoolRequest::Dimension& requestedDimension, const PoolRequest::Dimension& testDimension );

private:
	friend class ScopedTexture;
	void put( TextureHolderPtr pRecylingTexture );
	typedef std::list<TextureHolderPtr> RecyclingTexturePtrList;
	typedef std::map<PoolRequest, RecyclingTexturePtrList, PoolRequestLess> MAP;
	MAP m_Map;
};

class ScopedTexture : public ::boost::noncopyable
{
private:
	friend class TexturePool;
	TexturePool* const m_pPool;
	const TexturePool::TextureHolderPtr m_pRecylingTexture;
	ScopedTexture( TexturePool* const pPool, TexturePool::TextureHolderPtr pRecylingTexture );

public: ScopedTexture();
	~ScopedTexture();

	template<typename T>
	T* getTexture() const{
	    return m_pRecylingTexture ? dynamic_cast<T*>( m_pRecylingTexture->m_pResource.get() ) : NULL;
	}
};

#endif /* TEXTUREPOOL_H_ */
