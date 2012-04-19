#ifndef IIMAGEDESCRIPTOR_H_
#define IIMAGEDESCRIPTOR_H_

struct ImageDescription;

class IImageDescriptor
{
public:
    virtual ~IImageDescriptor(){}
	virtual const ImageDescription& getImageDescription() const = 0;
};

#endif // IIMAGEDESCRIPTOR_H_
