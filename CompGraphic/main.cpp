#include <vector>
#include <cmath>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);

Model* model = NULL;
const int width = 800;
const int height = 800;

void line(Vec2i p0, Vec2i p1, TGAImage& image, TGAColor color) {
    bool steep = false;
    if (std::abs(p0.x - p1.x) < std::abs(p0.y - p1.y)) {
        std::swap(p0.x, p0.y);
        std::swap(p1.x, p1.y);
        steep = true;
    }
    if (p0.x > p1.x) {
        std::swap(p0, p1);
    }

    for (int x = p0.x; x <= p1.x; x++) {
        float t = (x - p0.x) / (float)(p1.x - p0.x);
        int y = p0.y * (1. - t) + p1.y * t;
        if (steep) {
            image.set(y, x, color);
        }
        else {
            image.set(x, y, color);
        }
    }
}

void triangle(Vec2i t0, Vec2i t1, Vec2i t2,
    Vec2i uv0, Vec2i uv1, Vec2i uv2,
    TGAImage& image, float intensity, Model* model) {
    if (t0.y == t1.y && t0.y == t2.y) return;

    if (t0.y > t1.y) { std::swap(t0, t1); std::swap(uv0, uv1); }
    if (t0.y > t2.y) { std::swap(t0, t2); std::swap(uv0, uv2); }
    if (t1.y > t2.y) { std::swap(t1, t2); std::swap(uv1, uv2); }

    int total_height = t2.y - t0.y;

    for (int i = 0; i < total_height; i++) {
        bool second_half = i > t1.y - t0.y || t1.y == t0.y;
        int segment_height = second_half ? t2.y - t1.y : t1.y - t0.y;

        float alpha = (float)i / total_height;
        float beta = (float)(i - (second_half ? t1.y - t0.y : 0)) / segment_height;

        Vec2i A = t0 + (t2 - t0) * alpha;
        Vec2i B = second_half ? t1 + (t2 - t1) * beta : t0 + (t1 - t0) * beta;

        Vec2i uvA = uv0 + (uv2 - uv0) * alpha;
        Vec2i uvB = second_half ? uv1 + (uv2 - uv1) * beta : uv0 + (uv1 - uv0) * beta;

        if (A.x > B.x) { std::swap(A, B); std::swap(uvA, uvB); }

        for (int j = A.x; j <= B.x; j++) {
            float phi = (B.x == A.x) ? 1. : (float)(j - A.x) / (float)(B.x - A.x);
            Vec2i uvP = uvA + (uvB - uvA) * phi;

            TGAColor color = model->diffuse(uvP);
            // Применяем освещение
            color.r = (unsigned char)(color.r * intensity);
            color.g = (unsigned char)(color.g * intensity);
            color.b = (unsigned char)(color.b * intensity);

            image.set(j, t0.y + i, color);
        }
    }
}

int main(int argc, char** argv) {
    if (2 == argc) {
        model = new Model(argv[1]);
    }
    else {
        model = new Model("object.obj");
    }

    TGAImage image(width, height, TGAImage::RGB);
    Vec3f light_dir(0, 0, -1);  // Направление света

    for (int i = 0; i < model->nfaces(); i++) {
        std::vector<int> face = model->face(i);

        Vec2i screen_coords[3];
        Vec3f world_coords[3];
        Vec2i uv_coords[3];

        for (int j = 0; j < 3; j++) {
            Vec3f v = model->vert(face[j]);
            screen_coords[j] = Vec2i((v.x + 1.) * width / 2., (v.y + 1.) * height / 2.);
            world_coords[j] = v;
            uv_coords[j] = model->uv(i, j);
        }

        // Вычисляем нормаль и интенсивность освещения
        Vec3f n = (world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0]);
        n.normalize();
        float intensity = n * light_dir;

        // Рисуем только если треугольник видим (не отвернут от света)
        if (intensity > 0) {
            triangle(screen_coords[0], screen_coords[1], screen_coords[2],
                uv_coords[0], uv_coords[1], uv_coords[2],
                image, intensity, model);
        }
    }

    image.flip_vertically();
    image.write_tga_file("output.tga");

    delete model;
    return 0;
}