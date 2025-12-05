#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <iostream>
#include <cmath>

template <class t> struct Vec2 {
    t x, y;

    // Конструкторы
    Vec2() : x(t()), y(t()) {}
    Vec2(t _x, t _y) : x(_x), y(_y) {}

    // Операторы
    Vec2<t>& operator=(const Vec2<t>& v) {
        if (this != &v) {
            x = v.x;
            y = v.y;
        }
        return *this;
    }

    Vec2<t> operator +(const Vec2<t>& V) const { return Vec2<t>(x + V.x, y + V.y); }
    Vec2<t> operator -(const Vec2<t>& V) const { return Vec2<t>(x - V.x, y - V.y); }
    Vec2<t> operator *(float f)          const { return Vec2<t>(x * f, y * f); }

    t& operator[](const int i) {
        if (i <= 0) return x;
        else return y;
    }

    const t& operator[](const int i) const {
        if (i <= 0) return x;
        else return y;
    }
};

template <class t> struct Vec3 {
    t x, y, z;

    // Конструкторы
    Vec3() : x(t()), y(t()), z(t()) {}
    Vec3(t _x, t _y, t _z) : x(_x), y(_y), z(_z) {}

    // Шаблонный конструктор преобразования типа
    template <class u> Vec3(const Vec3<u>& v) {
        x = t(v.x);
        y = t(v.y);
        z = t(v.z);
    }

    // Операторы
    Vec3<t>& operator=(const Vec3<t>& v) {
        if (this != &v) {
            x = v.x;
            y = v.y;
            z = v.z;
        }
        return *this;
    }

    Vec3<t> operator ^(const Vec3<t>& v) const {
        return Vec3<t>(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
    }

    Vec3<t> operator +(const Vec3<t>& v) const {
        return Vec3<t>(x + v.x, y + v.y, z + v.z);
    }

    Vec3<t> operator -(const Vec3<t>& v) const {
        return Vec3<t>(x - v.x, y - v.y, z - v.z);
    }

    Vec3<t> operator *(float f) const {
        return Vec3<t>(x * f, y * f, z * f);
    }

    t operator *(const Vec3<t>& v) const {
        return x * v.x + y * v.y + z * v.z;
    }

    float norm() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    Vec3<t>& normalize(t l = 1) {
        *this = (*this) * (l / norm());
        return *this;
    }

    t& operator[](const int i) {
        if (i == 0) return x;
        else if (i == 1) return y;
        else return z;
    }

    const t& operator[](const int i) const {
        if (i == 0) return x;
        else if (i == 1) return y;
        else return z;
    }
};

typedef Vec2<float> Vec2f;
typedef Vec2<int>   Vec2i;
typedef Vec3<float> Vec3f;
typedef Vec3<int>   Vec3i;

// Операторы вывода
template <class t>
std::ostream& operator<<(std::ostream& s, const Vec2<t>& v) {
    s << "(" << v.x << ", " << v.y << ")\n";
    return s;
}

template <class t>
std::ostream& operator<<(std::ostream& s, const Vec3<t>& v) {
    s << "(" << v.x << ", " << v.y << ", " << v.z << ")\n";
    return s;
}

#endif // GEOMETRY_H