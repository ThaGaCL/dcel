#include "mesh.h"

int main() {
    Mesh mesh;
    mesh.load();
    bool isValid = mesh.isTopologyValid();
    if (isValid) mesh.printDCEL();
    return 0;
}