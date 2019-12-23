
#include "rectangle.h"

/* 判断直线l1、l2是否相交 l1端点为a、b，l2端点为c、d */
bool line_cross(const float *a, const float *b, const float *c, const float *d) {
    auto *vec1 = new float[2]; vec1[0] = b[0]-a[0]; vec1[1] = b[1]-a[1];
    auto *vec2 = new float[2]; vec2[0] = c[0]-a[0]; vec2[1] = c[1]-a[1];
    auto *vec3 = new float[2]; vec3[0] = d[0]-a[0]; vec3[1] = d[1]-a[1];
    // l2端点是否在l1两侧
    bool r1 = (vec1[0] * vec2[1] - vec1[1] * vec2[0]) * (vec1[0] * vec3[1] - vec1[1] * vec3[0]) <= 0;
    vec1[0] = d[0]-c[0]; vec1[1] = d[1]-c[1];
    vec2[0] = a[0]-c[0]; vec2[1] = a[1]-c[1];
    vec3[0] = b[0]-c[0]; vec3[1] = b[1]-c[1];
    // l1端点是否在l2两侧
    bool r2 = (vec1[0] * vec2[1] - vec1[1] * vec2[0]) * (vec1[0] * vec3[1] - vec1[1] * vec3[0]) <= 0;
    return r1 && r2;
}
/* 点p在线段ab上 */
bool point_on_line(float *p, float *a, float *b){
    float vec1[2] = {p[0]-a[0], p[1]-a[1]};
    float vec2[2] = {b[0]-a[0], b[1]-a[1]};
    return (vec1[0]*vec2[1] - vec1[1]*vec2[0]) == 0;
}

/* 判断点p是否在矩形内部 */
bool point_in_rec(float* p, float *a, float *b, float *c, float *d){
    // 射线法
    // 沿着x轴形成一条射线
    float line_end[2] = {p[0]+10000000, p[1]};
    // 判断射线与倾斜矩形4边相交次数
    int num_cross = 0;
    if(line_cross(p, line_end, a, b)) num_cross++;
    if(line_cross(p, line_end, b, c)) num_cross++;
    if(line_cross(p, line_end, c, d)) num_cross++;
    if(line_cross(p, line_end, d, a)) num_cross++;
    // 偶数次在矩形外部, 奇数次在矩形内部
    return num_cross % 2 != 0;
}

// 自身是否与矩形rec相交
bool Rectangle::intersection_rec(Rectangle *rec) {
    return !(rec->top() < bottom() || rec->bottom() > top() || rec->right() < left() || rec->left() > right());
}

/* 自身是否与倾斜的矩形相交 */
bool Rectangle::intersection_dip_rec(float *a, float *b, float *c, float *d) {
    // 依次判断矩形4个端点是否在矩形内部, 若相交, 必然有点在矩形内部
    if(point_in_rec(a, top_left, top_right, bottom_right, bottom_left)) return true;
    if(point_in_rec(b, top_left, top_right, bottom_right, bottom_left)) return true;
    if(point_in_rec(c, top_left, top_right, bottom_right, bottom_left)) return true;
    if(point_in_rec(d, top_left, top_right, bottom_right, bottom_left)) return true;

    if(point_in_rec(top_left, a, b, c, d)) return true;
    if(point_in_rec(top_right, a, b, c, d)) return true;
    if(point_in_rec(bottom_right, a, b, c, d)) return true;
    return point_in_rec(bottom_left, a, b, c, d);
}

// 操作：n大于0取n，小于0取0
float op(float n){
    if(n >= 0) return n;
    else return 0;
}

/* 自身是否与圆相交 */
bool Rectangle::intersection_circle(float *o, float r) {
    float vector_v[2] = {abs(o[0] - _center[0]), abs(o[1] - _center[1]) };
    float vector_h[2] = {(top_right[0] - _center[0]), (top_right[1] - _center[1])};
    // v-h，并将负数分量设为0
    float vector_u[2] = {op(vector_v[0]-vector_h[0]), op(vector_v[1]-vector_h[1])};
    return (pow(vector_u[0], 2)+pow(vector_u[1], 2)) <= r*r;
}

/* 自身与矩形rec相交的面积 */
float Rectangle::intersection_area(Rectangle *rec) {
    if (intersection_rec(rec)) {
        float h = abs(min(top(), rec->top()) - max(bottom(), rec->bottom()));
        float w = abs(min(right(), rec->right()) - max(left(), rec->left()));
        float area = h * w;
        return area;
    }
    else return 0;
}

/* 与另一矩形合并后的面积 */
float Rectangle::merge_area(Rectangle *rec) {
    float h = abs(max(top(), rec->top()) - min(bottom(), rec->bottom()));
    float w = abs(max(right(), rec->right()) - min(left(), rec->left()));
    float area = h * w;
    return area;
}

/* 与另一矩形合并后面积的增量 */
float Rectangle::increase_area(Rectangle *rec) {
    return merge_area(rec)-area()-rec->area()+intersection_area(rec);
}

/* 与另一矩形合并 */
void Rectangle::Merge(Rectangle *rec) {
    _top = max(top(), rec->top());
    _bottom = min(bottom(), rec->bottom());
    _right = max(right(), rec->right());
    _left = min(left(), rec->left());
    bottom_left[0] = _left; bottom_left[1] = _bottom;
    top_right[0] = _right; top_right[1] = _top;
    bottom_right[0] = _right; bottom_right[1] = _bottom;
    top_left[0] = _left; top_left[1] = _top;
    _center[0] = (_left + _right)/2;
    _center[1] = (_top + _bottom)/2;
}


