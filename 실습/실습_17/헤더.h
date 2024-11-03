#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <stdexcept>

struct Vertex {
    GLfloat x, y, z;
    glm::vec3 color;
};

struct Face {
    unsigned int v1, v2, v3;
};

struct Model {
    std::vector<Vertex> vertices;
    std::vector<Face> faces;
};

void read_obj_file(const std::string& filename, Model& model) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Error opening file: " + filename);
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::istringstream iss(line);

        if (line[0] == 'v' && line[1] == ' ') {
            Vertex vertex;
            iss.ignore(2);
            if (!(iss >> vertex.x >> vertex.y >> vertex.z)) {
                throw std::runtime_error("Failed to parse vertex data in file: " + filename);
            }
            model.vertices.push_back(vertex);
        }
        else if (line[0] == 'f' && line[1] == ' ') { 
            Face face;
            unsigned int v1, v2, v3;

            int result = sscanf(line.c_str(), "f %u//%*u %u//%*u %u//%*u", &v1, &v2, &v3);
            if (result == 3) {
                face.v1 = v1 - 1;
                face.v2 = v2 - 1;
                face.v3 = v3 - 1;
                model.faces.push_back(face);
            }
            else {
                throw std::runtime_error("Failed to parse face data in file: " + filename);
            }
        }
    }
}