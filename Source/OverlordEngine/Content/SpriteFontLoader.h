#pragma once
#include "ContentLoader.h"
#include "../Graphics/SpriteFont.h"

enum EBlockType
{
	INFO = 1,
	COMMON = 2,
	PAGES = 3,
	CHARS = 4,
	KERNINGPAIRS = 5
};

class SpriteFontLoader : public ContentLoader<SpriteFont>
{
public:
	SpriteFontLoader(void);
	~SpriteFontLoader(void);

protected:
	virtual SpriteFont* LoadContent(const wstring& assetFile);
	virtual void Destroy(SpriteFont* objToDestroy);

private:
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	SpriteFontLoader(const SpriteFontLoader& yRef);
	SpriteFontLoader& operator=(const SpriteFontLoader& yRef);
};