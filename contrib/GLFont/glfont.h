#ifndef _glfonth_
#define _glfonth_

//*********************************************************
//GLFONT.H -- Header for GLFONT.CPP
//Copyright (c) 1998 Brad Fish
//Copyright (c) 2002 Henri Kyrki
//See glFont.txt for terms of use
//10.5 2002
//*********************************************************

#ifndef TRUE
#define TRUE	1
#endif

#ifndef FALSE
#define FALSE	0
#endif

#include <string>

#ifdef _WIN32
#include <Windows.h>
#endif
#include <GL/gl.h>

namespace GLFontError {
	struct InvalidFile{};
	struct InvalidFont{};
}

class GLFontBase {
public:
	GLFontBase();
	void Begin();
    void SetColor(float cr, float cg, float cb) { r = cr; g = cg; b = cb; }
    void SetScale(float s) { scale = s; }
	virtual ~GLFontBase();
protected:

	void CreateImpl(const std::string &FileName, GLuint Tex, bool PixelPerfect = FALSE);

	typedef struct
	{
	union {
		float dx;
		int width;
	};
	union {
		float dy;
		int height;
	};
	float tx1, ty1;
	float tx2, ty2;
	} GLFONTCHAR;

	typedef struct
	{
	int Tex;
	int TexWidth, TexHeight;
	int IntStart, IntEnd;

	GLFONTCHAR *Char;
	} GLFONT;

	GLFONT Font;
	bool ok;
    float scale;
    float r, g, b;
private:
	void FreeResources();
};

class GLFont : public GLFontBase {
public:
	GLFont();
	void Create(const std::string &FileName, GLuint Tex);
	void PrintText(std::string String, float x, float y, float z);
};

class PixelPerfectGLFont : public GLFontBase {
public:
	PixelPerfectGLFont();
	void Create(const std::string &FileName, GLuint Tex);
	void PrintText(std::string String, int x, int y, int z);
};

#endif
//End of file

