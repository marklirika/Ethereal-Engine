#include "terrain.h"

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

        int numVerticesX = map_size.x / unit_size.x;
        int numVerticesY = map_size.y / unit_size.y;
        int numTrianglesX = numVerticesX - 1;
        int numTrianglesY = numVerticesY - 1;

        // Generate vertices
        float yoffset = 0;
        for (int y = 0; y < numVerticesY; y++) {
            float xoffset = 0;
            for (int x = 0; x < numVerticesX; x++) {
                float xPos = x * unit_size.x;
                float yPos = y * unit_size.y;
                OpenSimplexNoise::Noise noise(199);
                double zPos = glm::mix(0, 100, noise.eval(xoffset, -yoffset));
                if(zPos < 0) zPos = 0;
                vertices.push_back({ glm::vec3(xPos, yPos, zPos), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f) });
                xoffset += 0.01f;
            }
            yoffset += 0.01f;
        }

        // Generate indices
        for (int y = 0; y < numTrianglesY; y++) {
            for (int x = 0; x < numTrianglesX; x++) {
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