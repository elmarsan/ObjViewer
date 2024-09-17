#ifndef H_WAVEFRONT
#define H_WAVEFRONT

#include <fstream>

#include "types.h"

struct Wavefront
{
	~Wavefront();

	void load(std::istream& stream);
	void render();

	u32 VAO, EBO, VBO;
	u32 indicesCount;
};

#endif