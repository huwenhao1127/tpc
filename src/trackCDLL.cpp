#include "trackCDLL.h"

using std::cout;
using std::endl;

// 接口函数
extern "C" {
char * model_wrap(float *x, float *y, int length, int a){
    static std::string res;
    bool algorithm = true;
    if(a==0) algorithm = false;
    TrackC *model = new TrackC(algorithm);
    // 读取待聚类数据
    fmat data(length, 2);
    for (int i=0; i<length; i++){
        data(i, 0) = x[i];
        data(i, 1) = y[i];
    }
    res = model->fit_predict(data);
    return const_cast<char*>(res.c_str());
}
}

extern "C" {
char * direction_test_wrap(float *x, float *y, int length){
    fmat res;

    TrackC *model = new TrackC();
    // 读取待聚类数据
    fmat data(length, 2);
    for (int i=0; i<length; i++){
        data(i, 0) = x[i];
        data(i, 1) = y[i];
    }
    res = model->direction_test_region(data);
    std::string res_str = "";
    for (int i=0; i<res.n_cols; i++){
        res_str = res_str + std::to_string(res(0,i)) + ",";
        res_str = res_str + std::to_string(res(1,i)) + "&";
    }
    cout<<"cpp: "<<const_cast<char*>(res_str.c_str())<<endl;

    return const_cast<char*>(res_str.c_str());
}
}


