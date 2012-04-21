#ifndef ISHADERBASE_H_
#define ISHADERBASE_H_

#include "Enums.h"
#include "ResourceCache.h"
#include "math/Matrix4.h"
#include <Cg/cg.h>
#include <string>

class IShaderBase : public ::resource::IResource
{
public: IShaderBase( CGprogram program, TShaderType type );
	virtual ~IShaderBase();

	CGprogram   getProgram() const;
	TShaderType getType() const;

	void setParameter( const std::string& name, const float* value, const int size, const bool logIfInexistent = true ) const;
	const std::vector<std::string>& getParameterNames() const;
	CGparameter  getParameter( const std::string& name ) const;

protected:
	virtual void setParameter( const CGparameter parameter, const float* value, const int size ) const = 0;
	void appendParameter( const CGparameter );
	CGprogram m_Program;
	const TShaderType m_Type;
	std::vector<std::string> m_ParameterNames;
};

#include <boost/shared_ptr.hpp>
typedef ::boost::shared_ptr<IShaderBase> ShaderPtr;

#endif /* ISHADERBASE_H_ */
