#include <vector>
#include <cmath>
<<<<<<< Updated upstream
=======
#include <cstdlib> 
#include <limits>
<<<<<<< Updated upstream
>>>>>>> Stashed changes
=======
>>>>>>> Stashed changes
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
<<<<<<< Updated upstream
=======
const TGAColor green = TGAColor(0, 255, 0, 255);

<<<<<<< Updated upstream
>>>>>>> Stashed changes
=======
>>>>>>> Stashed changes
Model* model = NULL;
int* zbuffer = NULL;
Vec3f light_dir(0, 0, -1);

const int width = 800;
const int height = 800;
const int depth = 255;

void line(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color) {
    bool steep = false;
    if (std::abs(x0 - x1) < std::abs(y0 - y1)) {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }


    for (int x = x0; x <= x1; x++) {
        float t = (x - x0) / (float)(x1 - x0);
        int y = y0 * (1. - t) + y1 * t;
        if (steep) {
            image.set(y, x, color);
        }
        else {
            image.set(x, y, color);
        }
    }
}

<<<<<<< Updated upstream
<<<<<<< Updated upstream
=======
=======
>>>>>>> Stashed changes
void triangle(
    Vec3i t0, Vec3i t1, Vec3i t2, TGAImage& image, TGAColor color, int *zbuffer) {
    if (t0.y == t1.y && t0.y == t2.y) return; 
    if (t0.y > t1.y) std::swap(t0, t1);
    if (t0.y > t2.y) std::swap(t0, t2);
    if (t1.y > t2.y) std::swap(t1, t2);
    int total_height = t2.y - t0.y;
    for (int i = 0; i < total_height; i++) {
        bool second_half = i > t1.y - t0.y || t1.y == t0.y;
        int segment_height = second_half ? t2.y - t1.y : t1.y - t0.y;
        float alpha = (float)i / total_height;
        float beta = (float)(i - (second_half ? t1.y - t0.y : 0)) / segment_height; 
        Vec3i A = t0 + Vec3i(t2 - t0) * alpha;
        Vec3i B = second_half ? t1 + Vec3i(t2 - t1) * beta : t0 + Vec3i(t1 - t0) * beta;
        if (A.x > B.x) std::swap(A, B);
        for (int j = A.x; j <= B.x; j++) {
            float phi = B.x == A.x ? 1. : (float)(j - A.x) / (float)(B.x - A.x);
            Vec3i P = Vec3i(A) + Vec3i(B - A) * phi;
            int idx = P.x + P.y * width;
            if (zbuffer[idx] < P.z) {
                zbuffer[idx] = P.z;
                image.set(P.x, P.y, color);
            }
        }
    }
}

>>>>>>> Stashed changes
int main(int argc, char** argv) {
    if (2 == argc) {
        model = new Model(argv[1]);
    }
    else {
        model = new Model("object1.obj");
    }

<<<<<<< Updated upstream
<<<<<<< Updated upstream
    TGAImage image(width, height, TGAImage::RGB);
    for (int i = 0; i < model->nfaces(); i++) {
        std::vector<int> face = model->face(i);
        for (int j = 0; j < 3; j++) {
            Vec3f v0 = model->vert(face[j]);
            Vec3f v1 = model->vert(face[(j + 1) % 3]);
            int x0 = (v0.x + 1.) * width / 2.;
            int y0 = (v0.y + 1.) * height / 2.;
            int x1 = (v1.x + 1.) * width / 2.;
            int y1 = (v1.y + 1.) * height / 2.;
            line(x0, y0, x1, y1, image, white);
        }
    }

    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");
=======
    zbuffer = new int[width * height];

    for (int i = 0; i < width * height; i++) {
        zbuffer[i] = std::numeric_limits<int>::min();
    }

=======
    zbuffer = new int[width * height];

    for (int i = 0; i < width * height; i++) {
        zbuffer[i] = std::numeric_limits<int>::min();
    }

>>>>>>> Stashed changes
    {
        TGAImage image(width, height, TGAImage::RGB);

        for (int i = 0; i < model->nfaces(); i++) {
            std::vector<int> face = model->face(i);
            Vec3i screen_coords[3];
            Vec3f world_coords[3];
            for (int j = 0; j < 3; j++) {
                Vec3f v = model->vert(face[j]);
                screen_coords[j] = Vec3i((v.x + 1.) * width / 2., (v.y + 1.) * height / 2., (v.y + 1.) * depth / 2.);
                world_coords[j] = v;
            }
            Vec3f n = (world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0]);
            n.normalize();
            float intensity = n * light_dir;
            if (intensity > 0) {
                triangle(screen_coords[0], screen_coords[1], screen_coords[2], image, TGAColor(intensity * 255, intensity * 255, intensity * 255, 255), zbuffer);
            }
        }

        image.flip_vertically();
        image.write_tga_file("output.tga");
    }
<<<<<<< Updated upstream
>>>>>>> Stashed changes
=======
>>>>>>> Stashed changes
    delete model;
    return 0;
}
