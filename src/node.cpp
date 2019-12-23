#include "node.h"

float Entry::increase_area(Entry *e) {
    return mbr()->increase_area(e->mbr());
}

/* 条目在矩形内部 */
bool Entry::in_rectangle(Rectangle* rec) {
    return !(_data[0]<rec->left() || _data[0]>rec->right() || _data[1]<rec->bottom() || _data[1]>rec->top());
}

/* 条目在倾斜矩形内部： 射线法 */
bool Entry::in_dip_rectangle(float *a, float *b, float *c, float *d) {
    // 沿着x轴形成一条射线
    float line_end[2] = {_data[0]+10000000, _data[1]};
    // 判断射线与倾斜矩形4边相交次数
    int num_cross = 0;
    if(line_cross(_data, line_end, a, b)) num_cross++;
    if(line_cross(_data, line_end, b, c)) num_cross++;
    if(line_cross(_data, line_end, c, d)) num_cross++;
    if(line_cross(_data, line_end, d, a)) num_cross++;
    // 偶数次在矩形外部, 奇数次在矩形内部
    return num_cross % 2 != 0;
}

/* 条目在倾斜矩形边界上 */
bool Entry::on_dip_rectangle(float *a, float *b, float *c, float *d) {
    // 判断点是否在矩形的4条边上
    return point_on_line(_data, a, b) || point_on_line(_data, b, c) || point_on_line(_data, c, d) || point_on_line(_data, d, a);
}

/* 条目在圆内部 */
bool Entry::in_circle(float *o, float r) {
    return (pow((_data[0]-o[0]),2) + pow((_data[1]-o[1]), 2)) <= pow(r, 2);
}

void Node::AddNode(Node* n){
    _non_leaf_children.push_back(n);
    n->SetParent(this);
    _leaf_children.clear();
    _used_space = _non_leaf_children.size();
}
void Node::AddNode(vector<Node *> &e) {
    for(auto i:e) {
        _non_leaf_children.push_back(i);
        i->SetParent(this);
    }
    _leaf_children.clear();
    _used_space = _non_leaf_children.size();
}

void Node::AddEntry(vector<Entry *>& e) {
    for(auto i:e) _leaf_children.push_back(i);
    _non_leaf_children.clear();
    _used_space = _leaf_children.size();
}
void Node::AddEntry(Entry *e) {
    _leaf_children.push_back(e);
    _non_leaf_children.clear();
    _used_space = _leaf_children.size();
}

/* 非叶节点：从当前节点分裂出一个新节点 */
Node *Node::SplitNode() {
    vector<Node*> seeds = PickSeedNode();
    vector<Node*> group_1 = {seeds[0]};
    vector<Node*> group_2 = {seeds[1]};
    // 分组
    for(auto child : _non_leaf_children){
        if(child == seeds[0] || child == seeds[1]) continue;
        // 为了不使分组出后出现下溢，需要保证节点少的那一组节点数至少为m
        if(group_1.size() > group_2.size() && _non_leaf_children.size()-group_1.size() == _min_space){
            group_2.push_back(child);
            continue;
        }
        if(group_2.size() > group_1.size() && _non_leaf_children.size()-group_2.size() == _min_space){
            group_1.push_back(child);
            continue;
        }
        float increment_1 = seeds[0]->increase_area(child);
        float increment_2 = seeds[1]->increase_area(child);
        if(increment_1 < increment_2) group_1.push_back(child);
        else group_2.push_back(child);
    }
    // 分裂
    // 更新旧节点
    _non_leaf_children.clear();
    AddNode(group_1);
    UpdateRectangle();
    // 得到新节点
    Node* new_node = new Node(_max_space);
    new_node->AddNode(group_2);
    new_node->SetHeight(height());
    new_node->UpdateRectangle();
    return new_node;
}

/* 叶节点：从当前节点分裂出一个新节点 */
Node *Node::SplitLeafNode() {
    vector<Entry*> seeds = PickSeedEntry();
    vector<Entry*> group_1 = {seeds[0]};
    vector<Entry*> group_2 = {seeds[1]};
    // 分组
    for(auto child : _leaf_children){
        if(child == seeds[0] || child == seeds[1]) continue;
        // 为了不使分组出后出现下溢，需要保证节点少的那一组节点数至少为m
        if(group_1.size() > group_2.size() && _leaf_children.size()-group_1.size() == _min_space){
            group_2.push_back(child);
            continue;
        }
        if(group_2.size() > group_1.size() && _leaf_children.size()-group_2.size() == _min_space){
            group_1.push_back(child);
            continue;
        }
        float increment_1 = seeds[0]->increase_area(child);
        float increment_2 = seeds[1]->increase_area(child);
        if(increment_1 < increment_2) group_1.push_back(child);
        else group_2.push_back(child);
    }
    // 分裂
    // 更新旧节点
    _leaf_children.clear();
    AddEntry(group_1);
    UpdateRectangle();
    // 得到新节点
    Node* new_node = new Node(_max_space);
    new_node->AddEntry(group_2);
    new_node->UpdateRectangle();
    return new_node;
}

/* 从子节点或条目中选择两个作为种子 */
vector<Entry *> Node::PickSeedEntry() {
    Entry* seed_1{};
    Entry* seed_2{};
    float max_increment = INT8_MIN;
    for(auto i:_leaf_children)
        for(auto j:_leaf_children) {
            float increment = i->increase_area(j);
            if (increment > max_increment) {
                max_increment = increment;
                seed_1 = i;
                seed_2 = j;
            }
        }
    vector<Entry *> r = {seed_1, seed_2};
    return r;
}

vector<Node *> Node::PickSeedNode() {
    Node *seed_1{};
    Node *seed_2{};
    float max_increment = INT8_MIN;
    for (auto i:_non_leaf_children)
        for (auto j:_non_leaf_children) {
            float increment = i->increase_area(j);
            if (increment > max_increment) {
                max_increment = increment;
                seed_1 = i;
                seed_2 = j;
            }
        }
    vector<Node *> r = {seed_1, seed_2};
    return r;
}

/* 添加新条目e后mbr面积的增量 */
float Node::increase_area(Entry *e) {
    return mbr()->increase_area(e->mbr());
}
float Node::increase_area(Node *n) {
    return mbr()->increase_area(n->mbr());
}

void Node::UpdateRectangle() {
    if(_used_space == 0) return;
    if(is_leaf()){
        _mbr = _leaf_children[0]->mbr()->copy();
        for(auto child : _leaf_children) _mbr->Merge(child->mbr());
    }
    else{
        _mbr = _non_leaf_children[0]->mbr()->copy();
        for(auto child : _non_leaf_children) _mbr->Merge(child->mbr());
    }
}

