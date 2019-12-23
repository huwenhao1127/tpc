#include "trackC.h"

static const float PI = acos(-1);
static const float EPS = 30;
static const float L = 50;
static const float W = 20;

template <typename T>
inline T square(T x) {return pow(x,2);}

float cal_distance(float x1, float y1, float x2, float y2){
    return sqrt( square(x2-x1) + square(y2-y1) );
}

frowvec TrackC::trans_coordinate(const frowvec& o, frowvec p){
    // 计算大圆距离
    float lng1 = o[0]; float lng2 = p[0];
    float lat1 = o[1]; float lat2 = p[1];
    float r_earth = 6371009;
    float diff_lng = (lng2-lng1)*PI/180;
    float w_lat1 = lat1*PI/180;
    float w_lat2 = lat2*PI/180;
    float diff_lat = w_lat2-w_lat1;
    float d = 2 * asin( sqrt( square(sin(diff_lat/2)) + cos(w_lat1)*cos(w_lat2)* square(sin(diff_lng/2)) ));
    d = d * r_earth;
    // 计算偏航角
    float b = tan(w_lat2/2 + PI/4);
    float c = tan(w_lat1/2 + PI/4);
    float x = log(b/c);
    float bearing = atan2(diff_lng, x);
    // 计算正交坐标系下的坐标
    Mat<float> res;
    res<<sin(bearing)*d<<cos(bearing)*d;
    return res;
}

void TrackC::to_plane_coordinate(fmat& data){
    frowvec o = min(data);
    for(int i=0; i<data.n_rows; i++)
        data.row(i) = trans_coordinate(o, data.row(i));
}

void TrackC::region_query(const frowvec& p, const fmat& data, vector<int> &neighbors, fmat& direct){
    direct = direction_test(p, data);

    // 若p不存在方向向量,直接返回,不做无效计算
    if(direct.is_empty()) return;

//    if(use_rtree){
//        // 计算矩形领域的4个顶点
//        // 计算以方向向量为坐标基的4个顶点
//        frowvec project_p = p * direct;
//        frowvec p_a = {project_p[0]-W, project_p[1]+L};
//        frowvec p_b = {project_p[0]+W, project_p[1]+L};
//        frowvec p_c = {project_p[0]+W, project_p[1]-L};
//        frowvec p_d = {project_p[0]-W, project_p[1]-L};
//        // 投影回正交坐标基
//        frowvec a = p_a * inv(direct);
//        frowvec b = p_b * inv(direct);
//        frowvec c = p_c * inv(direct);
//        frowvec d = p_d * inv(direct);
//        // 查询矩形领域
//        vector<Entry*> r = tree->Search(a, b, c, d);
//        for(auto i : r)
//            neighbors.push_back(i->index());
//    }
//    else{
        fvec dir_1 = direct.col(0);
        fvec dir_2 = direct.col(1);
        fvec p_projection_1(data.n_rows);
        p_projection_1.fill(as_scalar(p * dir_1));
        fvec p_projection_2(data.n_rows);
        p_projection_2.fill(as_scalar(p * dir_2));

        fvec data_projection_1 = data * dir_1;
        fvec data_projection_2 = data * dir_2;

        fvec diff_1 = abs(p_projection_1 - data_projection_1);
        fvec diff_2 = abs(p_projection_2 - data_projection_2);

        for(int i=0; i<diff_1.n_rows; i++){
            if(diff_1[i] == 0 && diff_2[i] == 0) continue;
            if(diff_1[i] < L && diff_2[i] < W) neighbors.push_back(i);
        }
//    }
}

void TrackC::region_query_eps(const frowvec& p, const fmat& data, vector<frowvec> &neighbors){
    // 基于rtree的查询
    if(use_rtree){
        float o[2] = {p[0], p[1]};
        vector<Entry*> r =  tree->Search(o, EPS);
        for(auto i : r){
            frowvec vec_data = {i->data()[0], i->data()[1]};
            neighbors.emplace_back(vec_data);
        }
    }
    // 暴力搜索
    else{
        for(int i=0; i<data.n_rows; i++){
            float d = cal_distance(p[0], p[1], data.row(i)[0], data.row(i)[1]);
            if(d < EPS) neighbors.emplace_back(data.row(i));
        }
    }
}

fmat  TrackC::direction_test(const frowvec& p, const fmat& data){
    // 初始化方向向量为空矩阵
    fmat dir;
    // 先求出局部eps邻域
    vector<frowvec> neighbors;
    region_query_eps(p, data, neighbors);
    // 若邻域内没有元素则直接返回空矩阵
    if(neighbors.size() < 2) return dir;

    // 将neighbors转换成矩阵
    fmat _neighbors(neighbors.size(),2);
    for(unsigned long i=0; i<neighbors.size(); i++)
        _neighbors.row(i) = neighbors[i];

    // 按列计算均值

    frowvec _mean = mean(_neighbors);
    fmat _mean_mat(_neighbors.n_rows,2);
    for(int i=0; i<_neighbors.n_rows; _mean_mat.row(i++)=_mean);
    // 矩阵零均值化
    _neighbors = _neighbors-_mean_mat;
    // 计算协方差矩阵
    fmat _neighbors_T = _neighbors.t();
    fmat cov =  _neighbors_T * _neighbors;
    fmat n(2,2);
    n.fill(_neighbors.n_rows);
    cov = cov/n;
    // 计算协方差矩阵的特征值和特征向量
    fmat eigvec;
    fvec eigval;
    eig_sym(eigval, eigvec, cov);

    // 选择主方向
    dir.resize(2,2);
    if (eigval[0] > eigval[1]){
        dir.col(0) = eigvec.col(0);
        dir.col(1) = eigvec.col(1);
    }
    else{
        dir.col(0) = eigvec.col(1);
        dir.col(1) = eigvec.col(0);
    }
    return dir;
}

fmat TrackC::direction_test_region(const fmat& data){
    fmat dir;
    // 按列计算均值
    frowvec _mean = mean(data);
    fmat _mean_mat(data.n_rows,2);
    for(int i=0; i<data.n_rows; _mean_mat.row(i++)=_mean);
    // 矩阵零均值化
    fmat zero_mean_data;
    zero_mean_data = data - _mean_mat;
    // 计算协方差矩阵
    fmat data_T = zero_mean_data.t();
    fmat cov =  data_T * zero_mean_data;
    fmat n(2,2);
    n.fill(zero_mean_data.n_rows);
    cov = cov/n;
    // 计算协方差矩阵的特征值和特征向量
    fmat eigvec;
    fvec eigval;
    eig_sym(eigval, eigvec, cov);

    // 选择主方向
    dir.resize(2,2);
    if (eigval[0] > eigval[1]){
        dir.col(0) = eigvec.col(0);
        dir.col(1) = eigvec.col(1);
    }
    else{
        dir.col(0) = eigvec.col(1);
        dir.col(1) = eigvec.col(0);
    }
    return dir;
}

void TrackC::cluster(int cur_index, int k, const fmat& data, vector<frowvec> &cur_cluster){
    frowvec cur = data.row(cur_index);

    visited[cur_index] = 1;
    results_list[cur_index] = k;
    vector<int> neighbors = neighbors_list[cur_index];
    vector<int> seed_list(neighbors.begin(), neighbors.end());
    fmat dir = direction_list[cur_index];
    //cout<<"neighbors: "<<neighbors.size()<<endl;
    //cout<<"seeds: "<<seed_list.size()<<endl;
    while(!seed_list.empty()){
        cur_index = *(seed_list.end()-1);
        seed_list.pop_back();
        if(!visited[cur_index]){
            visited[cur_index] = 1;
            results_list[cur_index] = k;
            if(density_list[cur_index] >= threshold){
                neighbors = neighbors_list[cur_index];
                seed_list.insert(seed_list.end(), neighbors.begin(), neighbors.end());
            }
        }
    }
}

void TrackC::fit(fmat& data){
    // 初始化
    visited = new int[data.n_rows]();
    density_list = new float[data.n_rows]();

    // 1、将GPS数据投影到平面坐标系中
    to_plane_coordinate(data);

    // 建树
    if(use_rtree){
        for(int i=0; i<data.n_rows; i++){
            float d[2] = {data.row(i)[0], data.row(i)[1]};
            tree->Insert(i, d);
        }
        cout<<"建树结束"<<endl;
    }

    // 2、计算每个样本的自适应矩形领域
    float density = 0;
    float n = 0;
    for(int i=0; i<data.n_rows; i++){
        vector<int> neighbors;
        fmat direct;
        region_query(data.row(i), data, neighbors, direct);
        // 记录每个点的 自适应邻域、密度、局部方向
        neighbors_list.push_back(neighbors);
        density_list[i] = neighbors.size();
        direction_list.push_back(direct);

        if (density_list[i] > 2){
            n++;
            density += density_list[i];
        }
    }
    // 3、计算密度阈值
    //cout<<"非噪声点数目: "<<n<<endl;
    threshold = density/n;
    //cout<<"密度阈值: "<<std::setprecision(4)<<threshold<<endl;
    // 4、扩展簇
    int k = -1;
    results_list = vector<int>(data.n_rows, -1);
    vector<frowvec> cur_cluster;
    for(int i=0; i<data.n_rows; i++){
        if (density_list[i] > threshold){
            if(!visited[i]){
                k++;
                cluster(i, k, data, cur_cluster);
            }
        }
    }
    num_clusters = k+1;
    //cout<<"类别数: "<<k<<endl;
}

std::string TrackC::fit_predict(fmat& data){
    fit(data);
    std::string res;
    for (int i : results_list){
        res.append(std::to_string(i) + ",");
    }
    res = res + '*' + std::to_string(num_clusters);
    res = res + '*' + std::to_string(threshold);
    return res;
}


