#ifndef RTREE_H
#define RTREE_H

#include "node.h"
#include "armadillo"

using std::cout;
using std::endl;
using namespace arma;

class Rtree{
private:
    Node* _root;
    int _m;
    int _num_data;
    int _height;
public:
    explicit Rtree(int max_node_space){
        _m = max_node_space;
        _root = new Node(_m);
        _num_data = 0;
        _height = 1;
    };
    ~Rtree() = default;

    void print(){
        cout<<"num of data: "<<num_data()<<endl;
        cout<<"height: "<<height()<<endl;
        cout<<"  "<<endl;
        print(_root);
    }
    static void print(Node* n){
        n->print();
        if(!n->is_leaf()) for(auto i : n->non_leaf_children()) print(i);
    }

    Node* root(){return _root;};
    int num_data(){return _num_data;};
    int height(){return _height;};

    void Insert(Entry *e);
    void Insert(int index, float *p);
    void AdjustTree(Node* n);
    Node* ChooseLeaf(Entry *e);

    vector<Entry* > Search(Rectangle* rec);
    vector<Entry* > Search(float *a, float *b, float *c, float *d);
    vector<Entry* > Search(frowvec a, frowvec b, frowvec c, frowvec d);

    vector<Entry* > Search(float *o, float r);
};

#endif //RTREE_H
