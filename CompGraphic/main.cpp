#include <vector>
#include <cmath>
#include <cstring> // для memset
#include <limits>  // для numeric_limits
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
        float t = (float)(x - p0.x) / (float)(p1.x - p0.x);
        int y = (int)(p0.y * (1.0f - t) + p1.y * t);
        if (steep) {
            image.set(y, x, color);
        }
        else {
            image.set(x, y, color);
        }
    }
}

void triangle(Vec3i t0, Vec3i t1, Vec3i t2, Vec2i uv0, Vec2i uv1, Vec2i uv2, TGAImage& image, float intensity, int* zbuffer, Model* model) {
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
        Vec3i A = t0 + Vec3f(t2 - t0) * alpha;
        Vec3i B = second_half ? t1 + Vec3f(t2 - t1) * beta : t0 + Vec3f(t1 - t0) * beta;
        Vec2i uvA = uv0 + (uv2 - uv0) * alpha;
        Vec2i uvB = second_half ? uv1 + (uv2 - uv1) * beta : uv0 + (uv1 - uv0) * beta;

        if (A.x > B.x) {
            std::swap(A, B);
            std::swap(uvA, uvB);
        }

        for (int j = A.x; j <= B.x; j++) {
            float phi = (B.x == A.x) ? 1.0f : (float)(j - A.x) / (float)(B.x - A.x);

            // Интерполяция z и текстурных координат
            Vec3i P = Vec3f(A) + Vec3f(B - A) * phi;
            Vec2i uvP = uvA + (uvB - uvA) * phi;

            // ХАК: Исправляем координаты x и y из-за проблем с округлением float->int
            P.x = j;
            P.y = t0.y + i;

            // Теперь вычисляем индекс с исправленными координатами
            int idx = P.x + P.y * width;

            if (P.x >= 0 && P.x < width && P.y >= 0 && P.y < height && P.z > zbuffer[idx]) {
                zbuffer[idx] = P.z;

                TGAColor color = model->diffuse(uvP);

                // Применяем освещение
                color.r = (unsigned char)(color.r * intensity);
                color.g = (unsigned char)(color.g * intensity);
                color.b = (unsigned char)(color.b * intensity);

                image.set(P.x, P.y, color);
            }
        }
    }
}

int main(int argc, char** argv) {
    if (argc == 2) {
        model = new Model(argv[1]);
    }
    else {
        model = new Model("object.obj");
    }

    TGAImage image(width, height, TGAImage::RGB);

    // Инициализация z-буфера (минимальное значение)
    int* zbuffer = new int[width * height];
    for (int i = 0; i < width * height; i++) {
        zbuffer[i] = std::numeric_limits<int>::min();
    }

    Vec3f light_dir(0, 0, -1);

    for (int i = 0; i < model->nfaces(); i++) {
        std::vector<int> face = model->face(i);

        Vec3i screen_coords[3];
        Vec3f world_coords[3];
        Vec2i uv_coords[3];

        for (int j = 0; j < 3; j++) {
            Vec3f v = model->vert(face[j]);

            // Преобразуем мировые координаты в экранные
            // z масштабируем для z-буфера (можно настроить масштаб)
            screen_coords[j] = Vec3i(
                (int)((v.x + 1.0f) * width / 2.0f + 0.5f), // +0.5f для правильного округления
                (int)((v.y + 1.0f) * height / 2.0f + 0.5f),
                (int)((v.z + 1.0f) * 500.0f) // Можете попробовать разные масштабы
            );

            world_coords[j] = v;
            uv_coords[j] = model->uv(i, j);
        }

        // Вычисляем нормаль (проверьте порядок вершин!)
        Vec3f n = (world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0]);
        n.normalize();
        float intensity = n * light_dir;

        // Для отладки: можно добавить минимальную интенсивность
        if (intensity > 0.0f) {
            triangle(screen_coords[0], screen_coords[1], screen_coords[2],
                uv_coords[0], uv_coords[1], uv_coords[2],
                image, intensity, zbuffer, model);
        }
    }

    image.flip_vertically();
    image.write_tga_file("output.tga");

    // Очистка памяти
    delete[] zbuffer;
    delete model;

    return 0;
}