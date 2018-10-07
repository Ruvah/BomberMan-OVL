#include "stdafx.h"
#include "SpriteFontLoader.h"
#include "../Helpers/BinaryReader.h"
#include "../Content/ContentManager.h"
#include "../Graphics/TextRenderer.h"
#include "../Graphics/TextureData.h"

SpriteFontLoader::SpriteFontLoader()
{

}


SpriteFontLoader::~SpriteFontLoader()
{
}

#pragma warning( disable : 4702 )
SpriteFont* SpriteFontLoader::LoadContent(const wstring& assetFile)
{
	auto pBinReader = new BinaryReader(); //Prevent memory leaks
	pBinReader->Open(assetFile);

	if (!pBinReader->Exists())
	{
		delete pBinReader;
		Logger::LogFormat(LogLevel::Error, L"SpriteFontLoader::LoadContent > Failed to read the assetFile!\nPath: \'%s\'", assetFile.c_str());
		
		return nullptr;
	}

	//See BMFont Documentation for Binary Layout
	
	//Parse the Identification bytes (B,M,F)
	//If Identification bytes doesn't match B|M|F,
	//Log Error (SpriteFontLoader::LoadContent > Not a valid .fnt font) &
	//return nullptr
    string idBytes{};
    idBytes += pBinReader->Read<char>();
    idBytes += pBinReader->Read<char>();
    idBytes += pBinReader->Read<char>();

    if(idBytes != "BMF")
    {
        Logger::LogError(L"SpriteFontLoader::LoadContent > Not a valid .fnt font");
        return nullptr;
    }

	//Parse the version (version 3 required)
	//If version is < 3,
	//Log Error (SpriteFontLoader::LoadContent > Only .fnt version 3 is supported)
	//return nullptr
    int8_t versionByte = pBinReader->Read<int8_t>();

    if(versionByte < 3)
    {
        Logger::LogError(L"SpriteFontLoader::LoadContent > Only .fnt version 3 is supported");
        return nullptr;
    }

	//Valid .fnt file
	auto pSpriteFont = new SpriteFont();
	//SpriteFontLoader is a friend class of SpriteFont
	//That means you have access to its privates (pSpriteFont->m_FontName = ... is valid)

	//**********
	// BLOCK 1 *
	//**********
	//Retrieve the blockId and blockSize
	//Retrieve the FontSize (will be -25, BMF bug) [SpriteFont::m_FontSize]
	//Move the binreader to the start of the FontName [BinaryReader::MoveBufferPosition(...) or you can set its position using BinaryReader::SetBufferPosition(...))
	//Retrieve the FontName [SpriteFont::m_FontName]
    int8_t blockId = pBinReader->Read<int8_t>();
    int32_t blockSize = pBinReader->Read<int32_t>();

    int16_t fontSize = pBinReader->Read<int16_t>();
    pSpriteFont->m_FontSize = fontSize;

    pBinReader->MoveBufferPosition(12);
    wstring fontName = pBinReader->ReadNullString();
    pSpriteFont->m_FontName = fontName;
    

	//**********
	// BLOCK 2 *
	//**********
	//Retrieve the blockId and blockSize
	//Retrieve Texture Width & Height [SpriteFont::m_TextureWidth/m_TextureHeight]
	//Retrieve PageCount
	//> if pagecount > 1
	//> Log Error (SpriteFontLoader::LoadContent > SpriteFont (.fnt): Only one texture per font allowed)
	//Advance to Block2 (Move Reader)
    blockId = pBinReader->Read<int8_t>();
    blockSize = pBinReader->Read<int32_t>();

    pBinReader->MoveBufferPosition(4);

    int16_t texWidth = pBinReader->Read<int16_t>();
    pSpriteFont->m_TextureWidth = texWidth;
    int16_t texHeight = pBinReader->Read<int16_t>();
    pSpriteFont->m_TextureHeight = texHeight;
    int16_t pageCount = pBinReader->Read<int16_t>();
    if(pageCount > 1)
    {
        Logger::LogError(L"SpriteFontLoader::LoadContent > SpriteFont (.fnt): Only one texture per font allowed");
    }
    pBinReader->MoveBufferPosition(5);

	//**********
	// BLOCK 3 *
	//**********
	//Retrieve the blockId and blockSize
	//Retrieve the PageName (store Local)
	//	> If PageName is empty
	//	> Log Error (SpriteFontLoader::LoadContent > SpriteFont (.fnt): Invalid Font Sprite [Empty])
	//>Retrieve texture filepath from the assetFile path
	//> (ex. c:/Example/somefont.fnt => c:/Example/) [Have a look at: wstring::rfind()]
	//>Use path and PageName to load the texture using the ContentManager [SpriteFont::m_pTexture]
	//> (ex. c:/Example/ + 'PageName' => c:/Example/somefont_0.png)
    blockId = pBinReader->Read<int8_t>();
    blockSize = pBinReader->Read<int32_t>();

    wstring pageName = pBinReader->ReadNullString();
    if(pageName.empty())
    {
        Logger::LogError(L"SpriteFontLoader::LoadContent > SpriteFont (.fnt): Invalid Font Sprite [Empty]");
    }

    pageName = assetFile.substr(0, assetFile.rfind('/') + 1) + pageName;
    pSpriteFont->m_pTexture = ContentManager::Load<TextureData>(pageName);

	//**********
	// BLOCK 4 *
	//**********
	//Retrieve the blockId and blockSize
	//Retrieve Character Count (see documentation)
	//Retrieve Every Character, For every Character:
	//> Retrieve CharacterId (store Local) and cast to a 'wchar_t'
	//> Check if CharacterId is valid (SpriteFont::IsCharValid), Log Warning and advance to next character if not valid
	//> Retrieve the corresponding FontMetric (SpriteFont::GetMetric) [REFERENCE!!!]
	//> Set IsValid to true [FontMetric::IsValid]
	//> Set Character (CharacterId) [FontMetric::Character]
	//> Retrieve Xposition (store Local)
	//> Retrieve Yposition (store Local)
	//> Retrieve & Set Width [FontMetric::Width]
	//> Retrieve & Set Height [FontMetric::Height]
	//> Retrieve & Set OffsetX [FontMetric::OffsetX]
	//> Retrieve & Set OffsetY [FontMetric::OffsetY]
	//> Retrieve & Set AdvanceX [FontMetric::AdvanceX]
	//> Retrieve & Set Page [FontMetric::Page]
	//> Retrieve Channel (BITFIELD!!!) 
	//	> See documentation for BitField meaning [FontMetrix::Channel]
	//> Calculate Texture Coordinates using Xposition, Yposition, TextureWidth & TextureHeight [FontMetric::TexCoord]
    blockId = pBinReader->Read<int8_t>();
    blockSize = pBinReader->Read<int32_t>();

    int characterCount = blockSize / 20;

    for (int i = 0; i < characterCount; ++i)
    {
        wchar_t charId = static_cast<wchar_t>(pBinReader->Read<int32_t>());
        if(!SpriteFont::IsCharValid(charId))
        {
            Logger::LogWarning(L"Character not valid");
            continue;
        }
        FontMetric& metric = pSpriteFont->GetMetric(charId);
        metric.IsValid = true;
        metric.Character = charId;
        int16_t charX = pBinReader->Read<int16_t>();
        int16_t charY = pBinReader->Read<int16_t>();
        metric.Width = pBinReader->Read<int16_t>();
        metric.Height = pBinReader->Read<int16_t>();
        metric.OffsetX = pBinReader->Read<int16_t>();
        metric.OffsetY = pBinReader->Read<int16_t>();
        metric.AdvanceX = pBinReader->Read<int16_t>();
        metric.Page = pBinReader->Read<int8_t>();
        int8_t charChannel = pBinReader->Read<int8_t>();
        if (charChannel == 1) metric.Channel = 2;
        else if (charChannel == 2) metric.Channel = 1;
        else if (charChannel == 4) metric.Channel = 0;
        metric.TexCoord = XMFLOAT2{ charX / static_cast<float>(texWidth), charY / static_cast<float>(texHeight) };
    }

	//DONE :)

	delete pBinReader;
	return pSpriteFont;
	
	#pragma warning(default:4702)  
}

void SpriteFontLoader::Destroy(SpriteFont* objToDestroy)
{
	SafeDelete(objToDestroy);
}