#include "rtree.h"

void Rtree::AdjustTree(Node *n) {
    Node* pre_node = n;
    while(!pre_node->is_root()){
        // 当前节点是否溢出
        if(pre_node->is_overflow()){
            Node* parent = pre_node->parent();
            Node* new_node;
            if(pre_node->is_leaf()) new_node = pre_node->SplitLeafNode();
            else new_node = pre_node->SplitNode();
            parent->AddNode(new_node);
        }
        else pre_node->UpdateRectangle();
        pre_node = pre_node->parent();
    }
    // 调整根节点
    if(pre_node->is_overflow()) {
        Node* new_root = new Node(_m);
        Node* new_node;
        if(pre_node->is_leaf()) new_node = pre_node->SplitLeafNode();
        else new_node = pre_node->SplitNode();
        new_root->AddNode(new_node);
        new_root->AddNode(pre_node);
        new_root->SetHeight(pre_node->height()+1);
        _root = new_root;
        _height++;
    }
    _root->UpdateRectangle();
}

/* 选择mbr面积增加最小的节点 */
Node* Rtree::ChooseLeaf(Entry *e) {
    float min_increment = INT64_MAX;
    Node* pre_node = root();
    while(!pre_node->is_leaf()){
        min_increment = INT64_MAX;
        vector<Node*> candidate = pre_node->non_leaf_children();
        for (auto i : candidate){
            float increment = i->increase_area(e);
            if(increment < min_increment){
                min_increment = increment;
                pre_node = i;
            }
        }
    }
    return pre_node;
}

/* 插入一个新条目e */
void Rtree::Insert(Entry *e) {
    _num_data++;
    Node* leaf = ChooseLeaf(e);
    leaf->AddEntry(e);
    AdjustTree(leaf);
}

void Rtree::Insert(int index, float *p) {
    auto* e = new Entry(index, p);
    Insert(e);
}

void SearchResultInLeafNode(vector<Entry*>& result, Node* leaf, Rectangle* rec){
    for(auto i : leaf->leaf_children()) {
        if(i->in_rectangle(rec)) result.push_back(i);
    }
}
void SearchCandidateNode(vector<Node*>& candidate, Node* node, Rectangle* rec){
    for(auto i : node->non_leaf_children()){
        if(rec->intersection_rec(i->mbr())) candidate.push_back(i);
    }
}
vector<Entry *> Rtree::Search(Rectangle* rec) {
    vector<Node*> candidate = {_root};
    vector<Entry*> result;
    while(!candidate.empty()){
        Node* pre = *(candidate.end()-1);
        candidate.pop_back();
        if (pre->is_leaf()) SearchResultInLeafNode(result, pre, rec);
        else SearchCandidateNode(candidate, pre, rec);
    }
    return result;
}

void SearchResultInLeafNode(vector<Entry*>& result, Node* leaf, float *a, float *b, float *c, float *d){
    for(auto i : leaf->leaf_children()) {
        // 点在矩形内部
        if(i->in_dip_rectangle(a,b,c,d)) result.push_back(i);
        // 点在矩形边界上
        else if(i->on_dip_rectangle(a,b,c,d)) result.push_back(i);
        else continue;
    }
}
void SearchCandidateNode(vector<Node*>& candidate, Node* node, float *a, float *b, float *c, float *d){
    for(auto i : node->non_leaf_children()){
        if(i->mbr()->intersection_dip_rec(a,b,c,d)) candidate.push_back(i);
    }
}
vector<Entry *> Rtree::Search(float *a, float *b, float *c, float *d) {
    vector<Node*> candidate = {_root};
    vector<Entry*> result;
    while(!candidate.empty()){
        Node* pre = *(candidate.end()-1);
        candidate.pop_back();
        if (pre->is_leaf()) SearchResultInLeafNode(result, pre, a, b, c, d);
        else SearchCandidateNode(candidate, pre, a, b, c, d);
    }
    return result;
}

vector<Entry *> Rtree::Search(frowvec a, frowvec b, frowvec c, frowvec d) {
    float array_a[2] = {a[0], a[1]};
    float array_b[2] = {b[0], b[1]};
    float array_c[2] = {c[0], c[1]};
    float array_d[2] = {d[0], d[1]};
    return Search(array_a, array_b, array_c, array_d);
}

void SearchResultInLeafNode(vector<Entry*>& result, Node* leaf, float *o, float r){
    for(auto i : leaf->leaf_children()) {
        if(i->in_circle(o, r)) result.push_back(i);
    }
}
void SearchCandidateNode(vector<Node*>& candidate, Node* node, float *o, float r){
    for(auto i : node->non_leaf_children()){
        if(i->mbr()->intersection_circle(o, r)) candidate.push_back(i);
    }
}

vector<Entry *> Rtree::Search(float *o, float r) {
    vector<Node*> candidate = {_root};
    vector<Entry*> result;
    while(!candidate.empty()){
        Node* pre = *(candidate.end()-1);
        candidate.pop_back();
        if (pre->is_leaf()) SearchResultInLeafNode(result, pre, o, r);
        else SearchCandidateNode(candidate, pre, o, r);
    }
    return result;
}



