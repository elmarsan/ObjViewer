#include <glad/glad.h>

#include <string>
#include <sstream>
#include <array>
#include <vector>

#include "wavefront.h"
#include "math.h"

void Wavefront::load(std::istream& stream)
{
	std::vector<vec3> vertices;
	std::vector<vec3> normals;
	std::vector<u32> indices;

	std::string line;
	std::array<float, 3> components;

	while (std::getline(stream, line, '\n'))
	{
		std::stringstream strStream(line);
		std::string data;
		std::string dataType;
		int i = 0;

		while (std::getline(strStream, data, ' '))
		{
			if (i == 0)
			{
				dataType = data;

				// Comment or empty line, skip it
				if (dataType == "#" || data.empty())
				{
					break;
				}

				i++;
				continue;
			}

			try
			{
				components[i - 1] = std::stof(data);
				i++;
			}
			catch (std::invalid_argument const& ex)
			{
				std::cout << "Invalid data: " << ex.what() << std::endl;
			}
		}

		// Comment or empty line, skip it
		if (dataType == "#" || data.empty())
		{
			continue;
		}
		else if (dataType == "v")
		{
			vertices.emplace_back(components[0], components[1], components[2]);
		}
		else if (dataType == "f")
		{
			indices.emplace_back(static_cast<u32>(components[0]));
			indices.emplace_back(static_cast<u32>(components[1]));
			indices.emplace_back(static_cast<u32>(components[2]));
		}
		else if (dataType == "vn")
		{
			normals.emplace_back(components[0], components[1], components[2]);
		}
		else
		{
			std::cout << "Unknown component type: " << dataType << std::endl;
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