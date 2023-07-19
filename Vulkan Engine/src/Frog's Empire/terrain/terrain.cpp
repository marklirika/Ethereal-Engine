#include "terrain.h"
#include <iostream>
namespace Frogs_Empire {

	std::vector<ethereal::Entity> Terrain::generateObjects() {
		std::vector<ethereal::Entity> objects;
		//generate objects
		return objects;
	}

    std::unique_ptr<ethereal::EtherealModel> Terrain::generateTerrain(ethereal::EtherealDevice& device, glm::vec2 map_size, glm::vec2 unit_size) {
        using namespace ethereal;
        std::vector<EtherealModel::Vertex> vertices;
        std::vector<uint32_t> indices;
        int numVerticesX = int(map_size.x / unit_size.x);
        int numVerticesY = int(map_size.y / unit_size.y);
        int numTrianglesX = numVerticesX - 1;
        int numTrianglesY = numVerticesY - 1;
        OpenSimplexNoise::Noise noise(199);

        // Generate vertices
        float yoffset = 0;
        for (int y = 0; y < numVerticesY; y += 1) {
            float xoffset = 0;
            for (int x = 0; x < numVerticesX; x += 1) {
                float xPos = float(x) * unit_size.x;
                float yPos = float(y) * unit_size.y;
                float zPos = glm::mix(0, 100, noise.eval(xoffset, -yoffset));
                if (zPos < 0) zPos = 0;

                // Вычисление нормали
                glm::vec3 normal(0.0f); 
                if (x > 0 && x < numVerticesX - 1 && y > 0 && y < numVerticesY - 1) { 
                    glm::vec3 left = glm::vec3(xPos - unit_size.x, yPos, noise.eval(xoffset - 0.1f, -yoffset));
                    glm::vec3 right = glm::vec3(xPos + unit_size.x, yPos, noise.eval(xoffset + 0.1f, -yoffset));
                    glm::vec3 up = glm::vec3(xPos, yPos - unit_size.y, noise.eval(xoffset, -(yoffset - 0.01f)));
                    glm::vec3 down = glm::vec3(xPos, yPos + unit_size.y, noise.eval(xoffset, -(yoffset + 0.01f)));

                    glm::vec3 normal1 = glm::normalize(glm::cross(up - glm::vec3(xPos, yPos, zPos), left - glm::vec3(xPos, yPos, zPos)));
                    glm::vec3 normal2 = glm::normalize(glm::cross(right - glm::vec3(xPos, yPos, zPos), down - glm::vec3(xPos, yPos, zPos)));

                    normal = glm::normalize(normal1 + normal2);
                }
                vertices.push_back({ glm::vec3(xPos, yPos, zPos), glm::vec3(0.5f, 0.5f, 0.0f), {normal.x, normal.y, 0.7f} });
                xoffset += 0.01f;
            }
            yoffset += 0.01f;
        }

        // Generate indices
        for (int y = 0; y < numTrianglesY; y += 1) {
            for (int x = 0; x < numTrianglesX; x += 1) {
                uint32_t topLeft = y * numVerticesX + x;
                uint32_t topRight = topLeft + 1;
                uint32_t bottomLeft = (y + 1) * numVerticesX + x;
                uint32_t bottomRight = bottomLeft + 1;

                indices.push_back(topLeft);
                indices.push_back(bottomLeft);
                indices.push_back(topRight);

                indices.push_back(topRight);
                indices.push_back(bottomLeft);
                indices.push_back(bottomRight);
            }
        }

        // Generate terrain
        return EtherealModel::createModel(device, vertices, indices);
    }

} // namespace ethereal