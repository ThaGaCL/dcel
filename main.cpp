#include "mesh.h"

int main() {
    Mesh mesh;
    mesh.load();
    std::cout<< "isTopologyValid: " << mesh.isTopologyValid() << endl;
    return 0;
}