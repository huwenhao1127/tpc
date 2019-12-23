#ifndef NODE_H
#define NODE_H

#include <iostream>
#include "rectangle.h"
#include <vector>

using std::cout;
using std::endl;

class Entry{
private:
    float *_data;
    Rectangle *_mbr{};
    const float offset = 0.00000001;
    int _index;
public:
    explicit Entry(int index, float *d){
        _data = new float[2];
        _data[0] = d[0];
        _data[1] = d[1];
        _index = index;
        float bl[2] = {d[0]-offset, d[1]-offset};
        float tr[2] = {d[0]+offset, d[1]+offset};
        _mbr = new Rectangle(bl, tr);
    };
    ~Entry() = default;
    void print(){cout<<'('<<_data[0]<<','<<_data[1]<<')'<<endl;};
    Rectangle* mbr(){return _mbr;};
    float* data() {return _data;};
    int index() {return _index;};

    float increase_area(Entry* e);
    bool in_rectangle(Rectangle* rec);
    bool in_dip_rectangle(float *a, float *b, float *c, float *d);
    bool on_dip_rectangle(float *a, float *b, float *c, float *d);
    bool in_circle(float* o, float r);
};

class Node{
private:
    Node *_parent{};
    Rectangle *_mbr;
    vector<Node*> _non_leaf_children;
    vector<Entry*> _leaf_children;
    int _used_space;
    int _max_space;
    int _min_space;
    int _height;

public:
    explicit Node(int m){
        _mbr = nullptr;
        _max_space = m;
        _min_space = m/2;
        _height = 0;
        _used_space = 0;
    }
    ~Node() = default;

    void print(){
        cout<<"node: "<<this<<endl;
        cout<<"parent: "<<_parent<<endl;
        cout<<"used space: "<<_used_space<<","<<_non_leaf_children.size()<<','<<_leaf_children.size()<<endl;
        cout<<"height: "<<_height<<endl;
        cout<<"mbr: "<<_mbr->left()<<','<<_mbr->bottom()<<','<<_mbr->right()<<','<<_mbr->top()<<endl;
        cout<<" "<<endl;
    };

    Node* parent(){return _parent;};
    Rectangle* mbr(){return _mbr;};
    int height(){return _height;};
    bool is_overflow(){return (_used_space > _max_space);};
    bool is_underflow(){return (_used_space < _min_space);};
    bool is_root(){return !_parent;};
    bool is_leaf(){return !_height;};
    vector<Node*> non_leaf_children(){return _non_leaf_children;};
    vector<Entry*> leaf_children(){return _leaf_children;};
    void SetHeight(int n){_height = n;};
    void SetParent(Node* p){_parent = p;};

    void AddNode(Node* n);
    void AddNode(vector<Node* >& e);
    void AddEntry(vector<Entry* >& e);
    void AddEntry(Entry* e);
    void UpdateRectangle();

    float increase_area(Entry* e); // 叶节点添加新条目e后mbr面积的增量
    float increase_area(Node* n);  // 非叶节点添加新节点e后mbr面积的增量
    vector<Entry *> PickSeedEntry(); // 叶节点选择种子条目
    vector<Node *> PickSeedNode();   // 非叶节点选择种子节点
    Node* SplitNode();
    Node* SplitLeafNode();
};

#endif //NODE_H
