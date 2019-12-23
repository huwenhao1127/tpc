#ifndef READFILE_H
#define READFILE_H

#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

using namespace std;

template <class Type>
Type stringToNum(const std::string& str)
{
    std::istringstream iss(str);
    Type num;
    iss >> num;
    return num;
}


// 字符串分割
vector<string> split(const std::string& str, const std::string& delim);

// 从txt中读取不良驾驶行为坐标
template <typename T>
vector<T *> read_txt(const string& path){
    std::ifstream infile;
    infile.open(path.data());   //将文件流对象与文件连接起来
    assert(infile.is_open());   //若失败,则输出错误消息,并终止程序运行
    std::string s;
    std::vector<std::string> txt;
    // 逐行读入
    while(getline(infile,s)){
        txt.push_back(s);
    }
    // 初始化二维数组
    int num = (int)txt.size();
    std::vector<T*> data; //txt.size()个点
    static T *point; //vector是局部变量时可以直接做函数的返回值，但是vector里面的数组不可以，需要声明静态数组
    std::vector<std::string> r;
    for (int i=1; i<=num; i++){
        point = new T[2]; //申请一个新的内存空间，否则point一直指向一个内存地址，导致vector每一项都是同一个数组
        r = split(txt[i-1].c_str(), " ");
        point[0] = stringToNum<T>(r[0]);
        point[1] = stringToNum<T>(r[1]);
        data.push_back(point);
    }
    infile.close();             //关闭文件输入流
    return data;
};

// 将聚类结果写入txt文件
template <typename T>
void write_txt(T* r, int num, const string& path){
    ofstream OutFile(path);
    string str = "";
    for(int i=0; i<num-1; i++){
        str = str + std::to_string(r[i]) + ",";
    }
    str+=std::to_string(r[num-1]);
    OutFile << str;  //把字符串内容"This is a Test!"，写入Test.txt文件
    OutFile.close();
};
template <typename T>
void write_txt(std::vector<T> r, const string& path){
    ofstream OutFile(path);
    string str = "";
    for(int i=0; i<r.size()-1; i++){
        str = str + std::to_string(r[i]) + ",";
    }
    str+=std::to_string(r[r.size()-1]);
    OutFile << str;
    OutFile.close();
};

// 读csv，csv本质和txt是一样的，只不过第一行是表头
template <typename T>
vector<T*> read_csv(const string& path){
    std::fstream input(path);
    assert(input.is_open());   //若失败,则输出错误消息,并终止程序运行
    std::string s;
    std::vector<std::string> txt;
    getline(input,s);
    while(getline(input,s)){
        txt.push_back(s);
    }
    int num = (int)txt.size();
    std::vector<T*> res;    //txt.size()个点
    static T *point;        //vector是局部变量时可以直接做函数的返回值，但是vector里面的数组不可以，需要声明静态数组
    std::vector<int> label;
    std::vector<std::string> r;
    for (int i=1; i<=num; i++){
        point = new T[2]; //申请一个新的内存空间，否则point一直指向一个内存地址，导致vector每一项都是同一个数组
        r = split(txt[i-1].c_str(), ",");
        point[0] = stringToNum<T>(r[1]);
        point[1] = stringToNum<T>(r[0]);
        res.push_back(point);
    }
    input.close();
    return res;
};

// 将聚类结果写入txt文件
template <typename T>
void write_csv(T* r, int num, const string& path){
    ofstream OutFile(path);
    string str = "";
    for(int i=0; i<num-1; i++){
        str = str + std::to_string(r[i]) + ",";
    }
    str+=std::to_string(r[num-1]);
    OutFile << str;  //把字符串内容"This is a Test!"，写入Test.txt文件
    OutFile.close();
};
template <typename T>
void write_csv(std::vector<T* > r, const string& path){
    ofstream OutFile(path);
    // 写表头
    string str = "lng,lat";
    str += '\n';
    OutFile << str;
    // 写数据
    str = "";
    for(auto i : r){
        str = str + std::to_string(i[0]) + "," + std::to_string(i[1]) + '\n';
    }
    OutFile << str;
    OutFile.close();
};

#endif