#ifndef BINOMIALHEAP_BINOMIALHEAP_CPP
#define BINOMIALHEAP_BINOMIALHEAP_CPP

#include <memory>

class BinomialHeap{
private:
    class Buffer;
    class Node;
    
public:
    class ManipulationPointer{
    public:
        friend class BinomialHeap;

        ManipulationPointer(): buffer(nullptr) {}
        ~ManipulationPointer() {
            if (buffer != nullptr) {
                buffer.reset();
            }
        }
    private:
        std::shared_ptr<Buffer> buffer;
        
        ManipulationPointer(std::shared_ptr<Buffer> ourBuffer): buffer(ourBuffer) {}
    };

    BinomialHeap() {}

    ~BinomialHeap() {
        removeFromMemory(head_);
        head_.reset();
        minimum_.reset();
    }

    bool isEmpty() const {
        return head_ == nullptr;
    }

    ManipulationPointer insert(int key) {
        Node* node = new Node(key);
        std::shared_ptr<Node> nodeSharedPointer(node);
        Buffer* buffer = new Buffer(nodeSharedPointer);
        std::shared_ptr<Buffer> strongBuffer(buffer);
        node->buffer = strongBuffer;
        ManipulationPointer pointer(strongBuffer);
        BinomialHeap element(nodeSharedPointer);
        merge(element);
        return pointer;
    }

    int getMin() const {
        if (isEmpty()) {
            throw std::logic_error("There is no minimum in the empty heap");
        }
        return minimum_->value;
    }

    int extractMin() {
        if (isEmpty()) {
            throw std::logic_error("You can not delete from an empty heap");
        }
        int min = minimum_->value;
        if (head_ == minimum_) {
            head_ = head_->sibling;
            BinomialHeap children(minimum_->child);
            deleteMinimumFromMemory();
            merge(children);
            return min;
        }
        std::shared_ptr<Node> prevPointerToMin = head_;
        while (prevPointerToMin->sibling != minimum_) {
            prevPointerToMin = prevPointerToMin->sibling;
        }
        std::shared_ptr<Node> next = minimum_->sibling;
        prevPointerToMin->sibling = next;
        BinomialHeap children(minimum_->child);
        deleteMinimumFromMemory();
        merge(children);
        return min;
    }

    void merge(BinomialHeap& otherHeap) {
        if (head_ == nullptr) {
            head_ = otherHeap.head_;
            minimum_ = otherHeap.minimum_;
            otherHeap.head_ = nullptr;
            otherHeap.minimum_ = nullptr;
            return;
        } else if (otherHeap.head_ == nullptr) {
            return;
        }
        std::shared_ptr<Node> temporaryHeapList = nullptr;
        std::shared_ptr<Node> curHeapList = nullptr;
        std::shared_ptr<Node> heapList1 = head_;
        std::shared_ptr<Node> heapList2 = otherHeap.head_;
        if (head_->degree <= otherHeap.head_->degree) {
            temporaryHeapList = heapList1;
            heapList1 = heapList1->sibling;
        } else {
            temporaryHeapList = heapList2;
            heapList2 = heapList2->sibling;
        }
        curHeapList = temporaryHeapList;
        while (heapList1 != nullptr && heapList2 != nullptr) {
            if (heapList1->degree <= heapList2->degree) {
                curHeapList->sibling = heapList1;
                curHeapList = curHeapList->sibling;
                heapList1 = heapList1->sibling;
            } else {
                curHeapList->sibling = heapList2;
                curHeapList = curHeapList->sibling;
                heapList2 = heapList2->sibling;
            }
        }
        if (heapList1 == nullptr) {
            curHeapList->sibling = heapList2;
        } else {
            curHeapList->sibling = heapList1;
        }
        heapList1.reset();
        heapList2.reset();
        std::shared_ptr<Node> previousAnswerNode = nullptr;
        curHeapList = temporaryHeapList;
        std::shared_ptr<Node> ans = nullptr;
        while (curHeapList->sibling != nullptr) {
            if (curHeapList->degree != curHeapList->sibling->degree) {
                if (previousAnswerNode == nullptr) {
                    ans = curHeapList;
                }
                previousAnswerNode = curHeapList;
                curHeapList = curHeapList->sibling;
            } else if (curHeapList->sibling->sibling != nullptr && curHeapList->degree == curHeapList->sibling->sibling->degree) {
                if (previousAnswerNode == nullptr) {
                    ans = curHeapList;
                }
                previousAnswerNode = curHeapList;
                curHeapList = curHeapList->sibling;
            } else if (curHeapList->value <= curHeapList->sibling->value) {
                std::shared_ptr<Node> next = curHeapList->sibling;
                curHeapList->sibling = next->sibling;
                next->sibling.reset();
                mergeTree(curHeapList, next);
            } else {
                std::shared_ptr<Node> next = curHeapList->sibling;
                if (previousAnswerNode != nullptr) {
                    previousAnswerNode->sibling = next;
                }
                curHeapList->sibling.reset();
                mergeTree(next, curHeapList);
                curHeapList = next;
            }
        }
        if (previousAnswerNode == nullptr) {
            ans = curHeapList;
        }
        head_ = ans;
        curHeapList = head_;
        minimum_ = head_;
        while (curHeapList != nullptr) {
            if (minimum_->value > curHeapList->value) {
                minimum_ = curHeapList;
            }
            curHeapList = curHeapList->sibling;
        }
        otherHeap.head_.reset();
        otherHeap.minimum_.reset();
    }

    int getValue(ManipulationPointer ManipulationPointer) const {
        if (ManipulationPointer.buffer == nullptr) {
            throw std::invalid_argument("This ManipulationPointer is empty");
        }
        if (ManipulationPointer.buffer != nullptr && ManipulationPointer.buffer->deleted) {
            throw std::invalid_argument("This element has already been deleted");
        }
        return ManipulationPointer.buffer->node->value;
    }

    void change(ManipulationPointer ManipulationPointer, int key) {
        if (ManipulationPointer.buffer == nullptr) {
            throw std::invalid_argument("This ManipulationPointer is empty");
        }
        if (ManipulationPointer.buffer != nullptr && ManipulationPointer.buffer->deleted) {
            throw std::invalid_argument("This element has already been deleted");
        }
        std::shared_ptr<Node> cur = ManipulationPointer.buffer->node;
        if (key < getValue(ManipulationPointer)) {
            cur->value = key;
            std::shared_ptr<Node> parent = cur->parent;
            while (parent != nullptr && parent->value >= cur->value) {
                swapValues(cur, parent);
                cur = parent;
                parent = cur->parent;
            }
            if (parent == nullptr && key < minimum_->value) {
                minimum_ = cur;
            }
        } else if (key > getValue(ManipulationPointer)) {
            bool wasMin = cur == minimum_;
            cur->value = key;
            while (cur->child != nullptr) {
                std::shared_ptr<Node> smallestChild = cur->child;
                std::shared_ptr<Node> nowChild = cur->child;
                while (nowChild != nullptr) {
                    if (nowChild->value < smallestChild->value) {
                        smallestChild = nowChild;
                    }
                    nowChild = nowChild->sibling;
                }
                if (smallestChild->value >= key) {
                    return;
                }
                swapValues(cur,smallestChild);
                cur = smallestChild;
            }
            if (wasMin) {
                cur = head_;
                minimum_ = head_;
                while (cur != nullptr) {
                    if (minimum_->value > cur->value) {
                        minimum_ = cur;
                    }
                    cur = cur->sibling;
                }
            }
        }
    }
    void erase(ManipulationPointer ManipulationPointer) {
        if (ManipulationPointer.buffer == nullptr) {
            throw std::invalid_argument("This ManipulationPointer is empty");
        }
        if (ManipulationPointer.buffer != nullptr && ManipulationPointer.buffer->deleted) {
            throw std::invalid_argument("This element has already been deleted");
        }
        change(ManipulationPointer, minimum_->value);
        minimum_ = ManipulationPointer.buffer->node;
        extractMin();
    }
private:
    std::shared_ptr<Node> head_ = nullptr;
    std::shared_ptr<Node> minimum_ = nullptr;

    BinomialHeap(std::shared_ptr<Node> node) {
        head_ = node;
        minimum_ = head_;
        while (node != nullptr) {
            if(minimum_->value > node->value) {
                minimum_ = node;
            }
            node->parent.reset();
            node = node->sibling;
        }
    }

    class Node {
    public:
        friend class BinomialHeap;

        ~Node() {}
    private:
        int value;
        int degree;
        std::shared_ptr<Node> parent;
        std::shared_ptr<Node> child;
        std::shared_ptr<Node> sibling;
        std::shared_ptr<Buffer> buffer;

        Node(): degree(0), parent(nullptr), child(nullptr), sibling(nullptr), buffer(nullptr) {}
        Node(int element): value(element), parent(nullptr), child(nullptr), sibling(nullptr), degree(0) {}
    };
    class Buffer {
    public:
        friend class BinomialHeap;

        Buffer(std::shared_ptr<Node> nodePointer): node(nodePointer) {}
        ~Buffer() {
            node.reset();
        }
    private:
        std::shared_ptr<Node> node;
        bool deleted = false;
    };

    void removeFromMemory(std::shared_ptr<Node> curNode) {
        if (curNode == nullptr) {
            return;
        }
        std::shared_ptr<Node> child(curNode->child);
        std::shared_ptr<Node> sibling(curNode->sibling);
        std::shared_ptr<Buffer> buffer(curNode->buffer);
        curNode->child.reset();
        curNode->buffer.reset();
        curNode->parent.reset();
        curNode->sibling.reset();
        buffer->deleted = true;
        buffer->node.reset();
        curNode->buffer.reset();
        removeFromMemory(child);
        removeFromMemory(sibling);
        child.reset();
        sibling.reset();
        buffer.reset();
    }
    void mergeTree(std::shared_ptr<Node> head1, std::shared_ptr<Node> head2) {
        head1->degree++;
        head2->parent = head1;
        if (head1->child == nullptr) {
            head1->child = head2;
        } else {
            std::shared_ptr<Node>cur(head1->child);
            while (cur->sibling != nullptr) {
                cur = cur->sibling;
            }
            cur->sibling = head2;
        }
    }
    void swapValues(std::shared_ptr<Node> first, std::shared_ptr<Node> second) {
        std::shared_ptr<Buffer> buffer1(first->buffer);
        std::shared_ptr<Buffer> buffer2(second->buffer);
        int tmp = first->value;
        first->value = second->value;
        second->value = tmp;
        buffer1->node = second;
        buffer2->node = first;
        second->buffer = buffer1;
        first->buffer = buffer2;
        buffer1.reset();
        buffer2.reset();
    }
    void deleteMinimumFromMemory() {
        minimum_->sibling.reset();
        minimum_->child.reset();
        minimum_->buffer->deleted = true;
        minimum_->buffer->node.reset();
        minimum_->buffer.reset();
        minimum_.reset();
        minimum_ = head_;
    }
};

#endif //BINOMIALHEAP_BINOMIALHEAP_CPP