#include "Renderer.hpp"
#include "Scene.hpp"
#include "Triangle.hpp"
#include "Vector.hpp"
#include <chrono>

// In the main function of the program, we create the scene (create objects and
// lights) as well as set the options for the render (image width and height,
// maximum recursion depth, field-of-view, etc.). We then call the render
// function().
auto main(int argc, char** argv) -> int {

    // Change the definition here to change resolution
    Scene scene(784, 784);

    auto* red   = new Material(DIFFUSE, Vector3f(0.0F));
    red->Kd     = Vector3f(0.63F, 0.065F, 0.05F);
    auto* green = new Material(DIFFUSE, Vector3f(0.0F));
    green->Kd   = Vector3f(0.14F, 0.45F, 0.091F);
    auto* white = new Material(DIFFUSE, Vector3f(0.0F));
    white->Kd   = Vector3f(0.725F, 0.71F, 0.68F);
    auto* light =
        new Material(DIFFUSE, (8.0F * Vector3f(0.747F + 0.058F, 0.747F + 0.258F, 0.747F) +
                               15.6F * Vector3f(0.740F + 0.287F, 0.740F + 0.160F, 0.740F) +
                               18.4F * Vector3f(0.737F + 0.642F, 0.737F + 0.159F, 0.737F)));
    light->Kd = Vector3f(0.65F);

    MeshTriangle floor("./res/models/cornellbox/floor.obj", white);
    MeshTriangle shortbox("./res/models/cornellbox/shortbox.obj", white);
    MeshTriangle tallbox("./res/models/cornellbox/tallbox.obj", white);
    MeshTriangle left("./res/models/cornellbox/left.obj", red);
    MeshTriangle right("./res/models/cornellbox/right.obj", green);
    MeshTriangle light_("./res/models/cornellbox/light.obj", light);

    scene.Add(&floor);
    scene.Add(&shortbox);
    scene.Add(&tallbox);
    scene.Add(&left);
    scene.Add(&right);
    scene.Add(&light_);

    scene.buildBVH();

    Renderer r;

    auto start = std::chrono::system_clock::now();
    r.Render(scene);
    auto stop = std::chrono::system_clock::now();

    std::cout << "Render complete: \n";
    std::cout << "Time taken: "
              << std::chrono::duration_cast<std::chrono::hours>(stop - start).count() << " hours\n";
    std::cout << "          : "
              << std::chrono::duration_cast<std::chrono::minutes>(stop - start).count()
              << " minutes\n";
    std::cout << "          : "
              << std::chrono::duration_cast<std::chrono::seconds>(stop - start).count()
              << " seconds\n";

    return 0;
}
