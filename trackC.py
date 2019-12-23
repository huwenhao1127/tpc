from ctypes import *
import numpy as np
import sys

"""
@Parameter
    
@Example
    model = DBSCAN(eps=0.5, minpts=5, metric=0, algorithm=1)
    res = model.fit_predict(data)
    data是n*2的nparray
    res是长度为n的list，代表每个点的簇标记
@Created by 胡文豪 on 2019/10/12.
@References
    
"""

class Model:

    def __init__(self, _algorithm):
        self.labels = []
        self.data = None
        self.threshold = 0
        self.num_clusters = 0
        self.clusters = []
        self.bounds = []
        self.algorithm = _algorithm

    def fit(self, _data):
        self.data = _data
        path = ""
        if sys.platform == "darwin":
            path = "./build/libtpc.dylib"  
        else:
            path = "./build/libtpc.so" 
        lib = cdll.LoadLibrary(path)
        # 定义传入c的数组
        num_data = len(_data)
        array = c_float * num_data
        x = array()
        y = array()
        for i in range(num_data):
            x[i] = _data[i][0]
            y[i] = _data[i][1]
        # 定义c接口函数参数和返回值的数据类型
        lib.model_wrap.argtypes = [array, array, c_int, c_int]
        lib.model_wrap.restype = c_char_p
        res = lib.model_wrap(x, y, num_data, self.algorithm)
        # 对返回的结果字符串解码
        data = res.decode("utf-8")
        data = data.split('*')
        self.num_clusters = int(data[1])
        print('类别数：', self.num_clusters)
        self.threshold = float(data[2])
        print('阈值：', self.threshold)
        data = data[0]
        data = data.split(',')
        self.labels.clear()
        for i in data:
            if i != '':
                self.labels.append(int(i))

    def cal_bounds(self):
        self.clusters = []
        for i in range(self.num_clusters):
            self.clusters.append([])
        for i in range(len(self.labels)):
            if self.labels[i] != -1:
                self.clusters[self.labels[i]].append(self.data[i])
        for i in range(self.num_clusters):
            cluster = np.asarray(self.clusters[i])
            dir = self.dir_test(cluster)
            project = np.dot(cluster, dir)
            d1, d2, d3, d4 = max(project[:, 0]), min(project[:, 0]), max(project[:, 1]), min(project[:, 1])
            p1, p2, p3, p4 = np.array([d1, d4]), np.array([d1, d3]), np.array([d2, d3]), np.array([d2, d4])
            base = np.linalg.inv(dir)
            p1, p2, p3, p4 = np.dot(p1, base), np.dot(p2, base), np.dot(p3, base), np.dot(p4, base)
            self.bounds.append(np.asarray([p1.copy(), p2.copy(), p3.copy(), p4.copy()]))

    def fit_predict(self, _data):
        # 导入静态链接库
        self.fit(_data)
        self.cal_bounds()
        return self.labels, self.bounds

    def dir_test(self, _data):
        dir = []
        _mean = np.mean(_data, 0)
        _data = _data - _mean
        cov = np.dot(_data.T, _data)/len(_data)
        eigenvalue, featurevector = np.linalg.eig(cov)
        if eigenvalue[0] > eigenvalue[1]:
            dir.append(featurevector[:, 0])
            dir.append(featurevector[:, 1])
        else:
            dir.append(featurevector[:, 0])
            dir.append(featurevector[:, 1])
        dir = np.asarray(dir).T
        return dir
