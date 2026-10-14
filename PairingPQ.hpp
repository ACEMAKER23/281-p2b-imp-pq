// Project identifier: 43DE0E0C4C76BFAA6D8C2F5AEAE0518A9C261F4E

#ifndef PAIRINGPQ_H
#define PAIRINGPQ_H

#include <deque>
#include <utility>
#include <queue>
#include <vector>

#include "Eecs281PQ.hpp"
using namespace std;

// A specialized version of the priority queue ADT implemented as a pairing
// heap.
template<typename TYPE, typename COMP_FUNCTOR = std::less<TYPE>>
class PairingPQ : public Eecs281PQ<TYPE, COMP_FUNCTOR> {
    // This is a way to refer to the base class object.
    using BaseClass = Eecs281PQ<TYPE, COMP_FUNCTOR>;

public:
    // Each node within the pairing heap
    class Node {
    public:
        // Description: Custom constructor that creates a node containing
        //              the given value.
        explicit Node(const TYPE &val)
            : elt { val } {}

        // Description: Allows access to the element at that Node's position.
        //              There are two versions, getElt() and a dereference
        //              operator, use whichever one seems more natural to you.
        // Runtime: O(1) - this has been provided for you.
        const TYPE &getElt() const { return elt; }
        const TYPE &operator*() const { return elt; }

        // The following line allows you to access any private data
        // members of this Node class from within the PairingPQ class.
        // (ie: myNode.elt is a legal statement in PairingPQ's add_node()
        // function).
        friend PairingPQ;

    private:
        TYPE elt;
        Node *child = nullptr;
        Node *sibling = nullptr;
        Node *prev = nullptr;
        // TODO: Add and initialize one extra pointer (parent or previous) as desired.
    };  // Node


    // Description: Construct an empty pairing heap with an optional
    //              comparison functor.
    // Runtime: O(1)
    explicit PairingPQ(COMP_FUNCTOR comp = COMP_FUNCTOR())
        : BaseClass { comp }, root(nullptr), count(0) {
        // TODO: Implement this function.
    }  // PairingPQ()


    // Description: Construct a pairing heap out of an iterator range with an
    //              optional comparison functor.
    // Runtime: O(n) where n is number of elements in range.
    template<typename InputIterator>
    PairingPQ(InputIterator start, InputIterator end, COMP_FUNCTOR comp = COMP_FUNCTOR())
        : BaseClass { comp }, root(nullptr), count(0)  {
        // TODO: Implement this function.
        for(auto it = start; it != end; ++it){
            addNode(*it);
        }
    }  // PairingPQ()


    // Description: Copy constructor.
    // Runtime: O(n)
    PairingPQ(const PairingPQ &other)
        : BaseClass { other.compare }, root(nullptr), count(0) {
        // TODO: Implement this function.
        queue<Node*> processing;
        if (other.root) processing.push(other.root);
        while (!processing.empty())
        {
            Node* curr = processing.front();
            addNode(curr->elt);
            if (curr->child) processing.push(curr->child);
            if (curr->sibling) processing.push(curr->sibling);
            processing.pop();
        }
        
    }  // PairingPQ()


    // Description: Copy assignment operator.
    // Runtime: O(n)
    PairingPQ &operator=(const PairingPQ &rhs) {
        // TODO: Implement this function.
        // HINT: Use the copy-swap method from the "Arrays and Containers"
        // lecture.
        if (this == &rhs) {
            return *this;
        }
        PairingPQ t(rhs);
        swap(root, t.root);
        swap(count, t.count);
        swap(this->compare, t.compare);

        return *this;
    }  // operator=()


    // Description: Destructor
    // Runtime: O(n)
    ~PairingPQ() {
        // TODO: Implement this function.
        queue<Node*> processing;
        if (root) processing.push(root);
        while (!processing.empty())
        {
            Node* curr = processing.front();
            if (curr->child) processing.push(curr->child);
            if (curr->sibling) processing.push(curr->sibling);
            delete curr;
            processing.pop();
        }
        root = nullptr;
        count = 0;
    }  // ~PairingPQ()


    // Description: Move constructor and assignment operators don't need any
    //              code, the members will be reused automatically.
    PairingPQ(PairingPQ &&) noexcept = default;
    PairingPQ &operator=(PairingPQ &&) noexcept = default;


    // Description: Assumes that all elements inside the pairing heap are out
    //              of order and 'rebuilds' the pairing heap by fixing the
    //              pairing heap invariant.  You CANNOT delete 'old' nodes
    //              and create new ones!
    // Runtime: O(n)
    virtual void updatePriorities() {
        vector<Node*> allNode;
        queue<Node*> processing;

        if (root) processing.push(root);
        else return;

        while (!processing.empty())
        {
            Node* curr = processing.front();
            if (curr->child) processing.push(curr->child);
            if (curr->sibling) processing.push(curr->sibling);
            curr->child = nullptr;
            curr->sibling = nullptr;
            curr->prev = nullptr;
            allNode.push_back(curr);
            processing.pop();
        }

        Node* final = allNode.back();
        for (size_t i = allNode.size() - 1; i > 0; --i) {
            final = meld(allNode[i - 1], final);
        }

        root = final;

    }  // updatePriorities()


    // Description: Add a new element to the pairing heap. This is already
    //              done. You should implement push functionality entirely
    //              in the addNode() function, and this function calls
    //              addNode().
    // Runtime: O(1)
    virtual void push(const TYPE &val) { addNode(val); }  // push()


    // Description: Remove the most extreme (defined by 'compare') element
    //              from the pairing heap.
    // Note: We will not run tests on your code that would require it to pop
    // an element when the pairing heap is empty. Though you are welcome to
    // if you are familiar with them, you do not need to use exceptions in
    // this project.
    // Runtime: Amortized O(log(n))
    virtual void pop() {
        // TODO: Implement this function.
        vector<Node*> rootSib;
        Node* sib = root->child;
        Node* next = nullptr;

        while(sib){
            next = sib->sibling;
            isolate(sib);
            rootSib.push_back(sib);
            sib = next;
        }

        delete root;
        root = nullptr;
        --count;

        if (rootSib.empty()) {
            return;
        }

        vector<Node*> leftMerge;
        for(size_t i=0; i<rootSib.size()-1; i+=2){
            leftMerge.push_back(meld(rootSib[i],rootSib[i+1]));
        }
        if (rootSib.size()%2==1){
            leftMerge.push_back(rootSib.back());
        }

        Node* final = leftMerge.back();
        for (size_t i = leftMerge.size() - 1; i > 0; --i) {
            final = meld(leftMerge[i - 1], final);
        }

        root = final;

 
    }  // pop()


    // Description: Return the most extreme (defined by 'compare') element of
    //              the pairing heap. This should be a reference for speed.
    //              It MUST be const because we cannot allow it to be
    //              modified, as that might make it no longer be the most
    //              extreme element.
    // Runtime: O(1)
    virtual const TYPE &top() const {
        // TODO: Implement this function

        return root->elt;
    }  // top()


    // Description: Get the number of elements in the pairing heap.
    // Runtime: O(1)
    [[nodiscard]] virtual std::size_t size() const {
        // TODO: Implement this function
        return count;
    }  // size()

    // Description: Return true if the pairing heap is empty.
    // Runtime: O(1)
    [[nodiscard]] virtual bool empty() const {
        // TODO: Implement this function
        return count == 0;
    }  // empty()


    // Description: Updates the priority of an element already in the pairing
    //              heap by replacing the element refered to by the Node with
    //              new_value.  Must maintain pairing heap invariants.
    //
    // PRECONDITION: The new priority, given by 'new_value' must be more
    //              extreme (as defined by comp) than the old priority.
    //
    // Runtime: As discussed in reading material.
    void updateElt(Node *node, const TYPE &new_value) {

        // TODO: Implement this function
        node->elt = new_value;
        
        if (node == root) {
            return;
        }
        isolate(node);
        root = meld(root, node);
    }  // updateElt()


    // Description: Add a new element to the pairing heap. Returns a Node*
    //              corresponding to the newly added element.
    // Runtime: O(1)
    // NOTE: Whenever you create a node, and thus return a Node *, you must
    //       be sure to never move or copy/delete that node in the future,
    //       until it is eliminated by the user calling pop(). Remember this
    //       when you implement updateElt() and updatePriorities().
    Node *addNode(const TYPE &val) {
        // TODO: Implement this function
        Node* newNode = new Node(val);
        root = meld(root, newNode);
        ++count;
        return newNode;
    }  // addNode()


private:
    // TODO: Add any additional member variables or member functions you
    // require here.
    // TODO: We recommend creating a 'meld' function (see the Pairing Heap
    // papers).
    Node* meld(Node* R, Node* otherRoot){
        if (!otherRoot) return R;
        if (!R) return otherRoot;

        if (this->compare(otherRoot->elt, R->elt)){
            otherRoot->prev = R;
            otherRoot->sibling = R->child;

            if (R->child) {
                R->child->prev = otherRoot;
            }

            R->child = otherRoot;

            return R;
        }
        else{
            R->prev = otherRoot;
            R->sibling = otherRoot->child;

            if (otherRoot->child) {
                otherRoot->child->prev = R;
            }

            otherRoot->child = R;

            return otherRoot;
        }
    }

    void isolate(Node* alone){
        if (alone == root) return;
        if (alone->prev->child == alone){
            //im a left child
            alone->prev->child = alone->sibling;
            if (alone->sibling) alone->sibling->prev = alone->prev;

            alone->prev = nullptr;
            alone->sibling = nullptr;
        }
        else{
            //im a sibily of other
            alone->prev->sibling = alone->sibling;
            if (alone->sibling) alone->sibling->prev = alone->prev;

            alone->prev = nullptr;
            alone->sibling = nullptr;            
        }
    }
    // NOTE: For member variables, you are only allowed to add a "root
    //       pointer" and a "count" of the number of nodes. Anything else
    //       (such as a deque) should be declared inside of member functions
    //       as needed.
    Node* root;
    size_t count;

};


#endif  // PAIRINGPQ_H
