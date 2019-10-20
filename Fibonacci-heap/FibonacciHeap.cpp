#ifndef FIBONACCIHEAP_FIBONACCIHEAP_H
#define FIBONACCIHEAP_FIBONACCIHEAP_H

#include <memory>

const int MERGE_ARRAY_SIZE = 60; // this should be enought to hold 60! = 1 548 008 755 920 elements


class FibonacciHeap {
private:
    class Node;

public:
    class ManipulationPointer {
    public:
        friend class FibonacciHeap;

        ManipulationPointer(): node(nullptr) {}
        ~ManipulationPointer() {
            node.reset();
        }
    
    private:
        std::shared_ptr<Node> node;
        
        ManipulationPointer(std::shared_ptr<Node> _node): node(_node) {}
    };

    FibonacciHeap(): fib_trees_list_(nullptr) {}
    FibonacciHeap(std::shared_ptr<Node> node) {
        fib_trees_list_ = node;
        if (node != nullptr) {
            std::shared_ptr<Node> cur = node;
            while (cur->right != node) {
                cur->parent.reset();
                cur->marked = false;
                cur = cur->right;
            }
        }
    }
    ~FibonacciHeap() {
        std::shared_ptr<Node> curNode = fib_trees_list_;
        removeFromMemory(curNode);
        fib_trees_list_.reset();
        curNode.reset();
    }

    bool isEmpty() const {
        return fib_trees_list_ == nullptr;
    }
    
    int getMin() const {
        if (isEmpty()) {
            throw std::logic_error("There is no minimum in an empty heap");
        }
        return fib_trees_list_->key;
    }
    
    ManipulationPointer insert(int value) {
        std::shared_ptr<Node> curFibTree(new Node(value));
        curFibTree->left = curFibTree;
        curFibTree->right = curFibTree;
        if (isEmpty()) {
            fib_trees_list_ = curFibTree;
            ManipulationPointer ptr(curFibTree);
            return ptr;
        }
        FibonacciHeap otherHeap(curFibTree);
        merge(otherHeap);
        if (curFibTree->key < fib_trees_list_->key) {
            fib_trees_list_ = curFibTree;
        }
        ManipulationPointer ptr(curFibTree);
        return ptr;
    }
    
    int extractMin() {
        if (isEmpty()) {
            throw std::logic_error("There is no minimum in the empty heap");
        }
        std::shared_ptr<Node> min = fib_trees_list_;
        if (fib_trees_list_->child != nullptr) {
            FibonacciHeap otherHeap(fib_trees_list_->child);
            merge(otherHeap);
            fib_trees_list_->child.reset();
        }
        if (fib_trees_list_->left == fib_trees_list_) {
            fib_trees_list_.reset();
            return min->key;
        }
        fib_trees_list_->left->right = fib_trees_list_->right;
        fib_trees_list_->right->left = fib_trees_list_->left;
        fib_trees_list_ = fib_trees_list_->right;
        min->left.reset();
        min->right.reset();
        min->deleted = true;
        int value = min->key;
        consolidate();
        return value;
    }
    
    void decreaseKey(ManipulationPointer ptr, int key) {
        std::shared_ptr<Node> node = ptr.node;
        if (ptr.node == nullptr) {
            throw std::invalid_argument("Your ManipulationPointer is empty");
        }
        if (ptr.node->deleted) {
            throw std::invalid_argument("This element has already been deleted");
        }
        if (node->key < key) {
            throw std::invalid_argument("You can not decrease the key");
        }
        if (node->parent == nullptr) {
            node->key = key;
            if (key < fib_trees_list_->key) {
                fib_trees_list_ = node;
            }
            return;
        }
        if (node->parent->key < key) {
            node->key = key;
            return;
        }
        node->key = key;
        std::shared_ptr<Node> parent = node->parent;
        cut(node);
        cascadingCut(parent);
    }

    void merge(FibonacciHeap& otherHeap) {
        if (fib_trees_list_ == nullptr) {
            fib_trees_list_ = otherHeap.fib_trees_list_;
            return;
        }
        if (otherHeap.fib_trees_list_ == nullptr) {
            return;
        }
        mergeNodes(fib_trees_list_, otherHeap.fib_trees_list_);
        if (otherHeap.fib_trees_list_->key < fib_trees_list_->key) {
            fib_trees_list_ = otherHeap.fib_trees_list_;
        }
        otherHeap.fib_trees_list_.reset();
    }
    
    int getValue(ManipulationPointer ptr) const {
        if (ptr.node == nullptr) {
            throw std::invalid_argument("Your ManipulationPointer is empty");
        }
        if (ptr.node->deleted) {
            throw std::invalid_argument("This element has already been deleted");
        }
        return ptr.node->key;
    }
private:
    std::shared_ptr<Node> fib_trees_list_;

    class Node {
    public:
        friend class FibonacciHeap;

        ~Node() {
            parent.reset();
            child.reset();
            left.reset();
            right.reset();
        }
    private:
        int key;
        std::shared_ptr<Node> parent;
        std::shared_ptr<Node> child;
        std::shared_ptr<Node> left;
        std::shared_ptr<Node> right;
        int degree;
        bool marked;
        bool deleted;

        Node(int value): key(value), parent(nullptr), child(nullptr), left(nullptr), right(nullptr),
                         degree(0), marked(false), deleted(false) {}
    };

    void mergeNodes(std::shared_ptr<Node> first, std::shared_ptr<Node> second) {
        if (first == nullptr) {
            first = second;
            return;
        } else if (second == nullptr){
            return;
        }
        std::shared_ptr<Node> leftF = first->left;
        std::shared_ptr<Node> rightS = second->right;
        second->right = first;
        first->left = second;
        leftF->right = rightS;
        rightS->left = leftF;
        leftF.reset();
        rightS.reset();
    }
    
    void consolidate() {
        std::shared_ptr<Node> nodes[MERGE_ARRAY_SIZE];
        while (!isEmpty()) {
            std::shared_ptr<Node> curProcessingNode = nullptr;
            if (fib_trees_list_ == fib_trees_list_->right) {
                curProcessingNode = fib_trees_list_;
                fib_trees_list_.reset();
            } else {
                std::shared_ptr<Node> toTheRight = fib_trees_list_->right;
                curProcessingNode = fib_trees_list_;
                fib_trees_list_->left->right = fib_trees_list_->right;
                fib_trees_list_->right->left = fib_trees_list_->left;
                fib_trees_list_->left = fib_trees_list_;
                fib_trees_list_->right = fib_trees_list_;
                fib_trees_list_ = toTheRight;
                toTheRight.reset();
            }
            bool canProceed = true;
            while(canProceed) {
                if (nodes[curProcessingNode->degree] == nullptr) {
                    nodes[curProcessingNode->degree] = curProcessingNode;
                    canProceed = false;
                } else {
                    int deg = curProcessingNode->degree;
                    if(curProcessingNode->key <= nodes[curProcessingNode->degree]->key) {
                        curProcessingNode->degree++;
                        mergeNodes(curProcessingNode->child, nodes[deg]);
                        curProcessingNode->child = nodes[deg];
                        nodes[deg]->parent = curProcessingNode;
                        nodes[deg].reset();
                    } else {
                        nodes[deg]->degree++;
                        mergeNodes(curProcessingNode, nodes[deg]->child);
                        curProcessingNode->parent = nodes[deg];
                        nodes[deg]->child = curProcessingNode;
                        curProcessingNode = nodes[deg];
                        nodes[deg].reset();
                    }
                }
            }
        }
        int i = 0;
        while (nodes[i] == nullptr) {
            i++;
        }
        fib_trees_list_ = nodes[i];
        i++;
        while (i < MERGE_ARRAY_SIZE) {
            if (nodes[i] != nullptr) {
                FibonacciHeap otherHeap(nodes[i]);
                merge(otherHeap);
            }
            i++;
        }
    }
    
    void cut(std::shared_ptr<Node> node) {
        if (node->right == node) {
            node->parent->child = nullptr;
        } else {
            node->parent = node->right;
            node->right->left = node->left;
            node->left->right = node->right;
        }
        node->parent->degree--
;        node->left = node;
        node->right = node;
        FibonacciHeap otherHeap(node);
        merge(otherHeap);
    }
    
    void cascadingCut(std::shared_ptr<Node> node) {
        while (node->marked && node->parent != nullptr) {
            cut(node);
            node = node->parent;
        }
        node->marked = true;
    }
    
    void removeFromMemory(std::shared_ptr<Node> node) {
        if (node == nullptr) {
            return;
        }
        bool canProceed = true;
        std::shared_ptr<Node> curNode = node;
        while (canProceed) {
            if (curNode->parent != nullptr) {
                curNode->parent.reset();
            }
            if (curNode->right == curNode) {
                std::shared_ptr<Node> children = curNode->child;
                curNode->left.reset();
                curNode->right.reset();
                removeFromMemory(children);
                curNode->child.reset();
                curNode.reset();
                canProceed = false;
            } else {
                std::shared_ptr<Node> toTheLeft = curNode->left;
                std::shared_ptr<Node> toTheRight = curNode->right;
                std::shared_ptr<Node> children = curNode->child;
                toTheLeft->right = toTheRight;
                toTheRight->left = toTheLeft;
                removeFromMemory(children);
                curNode->child.reset();
                curNode->left.reset();
                curNode->right.reset();
                curNode = toTheRight;
            }
        }
    }
};

#endif //FIBONACCIHEAP_FIBONACCIHEAP_H