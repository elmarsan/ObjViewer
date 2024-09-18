#include <glad/glad.h>

#include <string>
#include <sstream>
#include <vector>
#include <regex>

#include "wavefront.h"
#include "math.h"

void Wavefront::load(std::istream& stream)
{
	std::vector<vec3> vertices;
	std::vector<vec3> normals;
	std::vector<u32> indices;
	std::vector<vec2> textures;

	std::string line;

	while (std::getline(stream, line, '\n'))
	{
		const std::regex wordsRegex("[^\\s]+");
		const auto itBegin = std::sregex_iterator(line.begin(), line.end(), wordsRegex);
		const auto itEnd = std::sregex_iterator();

		std::vector<std::string> data;

		for (std::sregex_iterator i = itBegin; i != itEnd; ++i)
		{
			std::smatch match = *i;
			data.push_back(match.str());
		}

		try
		{
			// Comment or empty line
			if (data.empty() || data[0].empty() || data[0] == "#")
			{
				continue;
			}
			// Vertex
			else if (data[0] == "v")
			{
				vertices.emplace_back(std::stof(data[1]), std::stof(data[2]), std::stof(data[3]));
			}
			// Indices
			// TODO: Parse complex indices
			else if (data[0] == "f")
			{
				indices.emplace_back(stoi(data[1]));
				indices.emplace_back(stoi(data[2]));
				indices.emplace_back(stoi(data[3]));
			}
			// Normal
			else if (data[0] == "vn")
			{
				normals.emplace_back(std::stof(data[1]), std::stof(data[2]), std::stof(data[3]));
			}
			// Texture
			else if (data[0] == "vt")
			{
				textures.emplace_back(std::stof(data[1]), std::stof(data[2]));
			}
			else
			{
				std::cout << "Unsupported component: " << line << std::endl;
			}
		}
		catch (std::invalid_argument const& ex)
		{
			std::cout << "Invalid data: " << line << std::endl;
		}
	}

	std::cout << "Wavefront object loaded\n";
	std::cout << "Number of vertices: " << vertices.size() << std::endl;
	std::cout << "Number of normals: " << normals.size() << std::endl;
	std::cout << "Number of indices: " << indices.size() << std::endl;

	indicesCount = indices.size();

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &EBO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), &indices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);
	glEnableVertexAttribArray(0);
}

void Wavefront::render()
{
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, 0);
}

Wavefront::~Wavefront()
{
	glDeleteBuffers(1, &EBO);
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
}