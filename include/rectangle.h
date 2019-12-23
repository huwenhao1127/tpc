#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "vector"
#include "iostream"
#include <cmath>

using std::vector;
using std::cout;
using std::endl;


bool line_cross(const float *a, const float *b, const float *c, const float *d);
bool point_on_line(float *p, float *a, float *b);

/* 矩形类*/
class Rectangle {
private:
    float bottom_left[2]{};
    float top_right[2]{};
    float bottom_right[2]{};
    float top_left[2]{};
    float _center[2]{};
    float _top;
    float _bottom;
    float _left;
    float _right;
public:
    Rectangle(const float *bl, const float *tr){
        _top = tr[1]; _bottom = bl[1]; _left = bl[0]; _right = tr[0];
        bottom_left[0] = _left; bottom_left[1] = _bottom;
        top_right[0] = _right; top_right[1] = _top;
        bottom_right[0] = _right; bottom_right[1] = _bottom;
        top_left[0] = _left; top_left[1] = _top;
        _center[0] = (_left + _right)/2;
        _center[1] = (_top + _bottom)/2;
    };
    ~Rectangle() = default;
    Rectangle* copy(){ return new Rectangle(bottom_left, top_right);};

    static inline float abs(float a){
        if(a>=0) return a;
        else return -a;
    };
    static inline float max(float a, float b){
        if(a < b) return b;
        else return a;
    };
    static inline float min(float a, float b){
        if(a < b) return a;
        else return b;
    };

    inline float area(){return abs(top_right[1]-bottom_left[1]) * abs(top_right[0]-bottom_left[0]);};
    inline float top(){return _top;};
    inline float bottom(){return _bottom;};
    inline float right(){return _right;};
    inline float left(){return _left;};

    inline float* center(){return _center;};

    vector<float> rec(){
        vector<float> r = {bottom_left[0], bottom_left[1], top_right[0], top_right[1]};
        return r;
    };

    // 是否与矩形相交
    bool intersection_rec(Rectangle *rec);

    // 是否与圆相交
    bool intersection_circle(float *o, float r);

    // 是否与倾斜矩形相交
    bool intersection_dip_rec(float *a, float *b, float *c, float *d);

    // 与另一矩形相交的面积
    float intersection_area(Rectangle *rec);

    // 合并后矩形的面积
    float merge_area(Rectangle *rec);

    // 合并后矩形的增量
    float increase_area(Rectangle *rec);

    // 与rec合并
    void Merge(Rectangle *rec);

};

#endif
