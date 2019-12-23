#ifndef trackC_h
#define trackC_h

#include <vector>
#include <armadillo>
#include <iostream>
#include <cmath>
#include <cstdio>
#include "rtree.h"

using std::vector;
using std::cout;
using std::endl;

using namespace arma;

class TrackC{
private:
    // 每个点的自适应矩形邻域
    vector<vector<int>> neighbors_list;
    // 按顺序存每个点的方向
    vector<fmat> direction_list;
    int* visited = nullptr;
    float* density_list = nullptr;
    
    /* 需要返回的结果 */
    // 每个元素的标签
    vector<int> results_list{};
    // 得到的密度阈值
    float threshold = 0;
    // 类别数
    int num_clusters = 0;
    // 所有簇的信息
    vector<mat> cluster_list;
    // 每个簇的边界信息
    vector<mat> bounds_list;
    bool use_rtree;
    Rtree* tree;

public:
    explicit TrackC(bool algorithm = true){
        use_rtree = algorithm;
        tree = new Rtree(16);
    };
    ~TrackC()= default;;
    // 转换p点坐标
    static frowvec trans_coordinate(const frowvec& o, frowvec p);
    // 将数据集映射到平面坐标系
    static void to_plane_coordinate(Mat<float>& data);
    // 计算p点的eps邻域
    void region_query_eps(const frowvec& p, const fmat& data, vector<frowvec> &neighbors);
    // 计算p点自适应矩形邻域
    void region_query(const frowvec& p, const fmat& data, vector<int> &neighbors, fmat& direct);
    // 计算p点局部的延伸方向,mat是2*2矩阵,第一列为主方向
    fmat direction_test(const frowvec& p, const fmat& data);
    // 计算区域的延伸方向
    static fmat direction_test_region(const fmat& data);
    // 簇扩展
    inline void cluster(int cur_index, int k, const fmat& data, vector<frowvec> &cur_cluster);
    // 计算边界

    void fit(fmat& data);
    // 传给接口函数的输出是字符串
    std::string fit_predict(fmat& data);
    //vector<int> fit_predict(mat &data);
};

#endif /* trackC_h */
