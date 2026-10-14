    // Project identifier: 43DE0E0C4C76BFAA6D8C2F5AEAE0518A9C261F4E

    /*
    * Compile this test against your .h files to make sure they compile. We
    * suggest adding to this file or creating your own test cases to test your
    * priority queue implementations more thoroughly. If you do not call a
    * function from here, that template is not instantiated and that function is
    * NOT compiled! So for instance, if you don't add code here to call
    * updatePriorities, that function could later cause compiler errors that you
    * don't even know about.
    *
    * Our makefile will build an executable named testPQ if you type 'make
    * testPQ' or 'make alltests' (without the quotes). This will be a debug
    * executable.
    *
    * Notice that testPairing tests the range-based constructor but main and
    * testPriorityQueue do not. Make sure to test the range-based constructor
    * for other PQ types, and also test the PairingPQ-specific member functions.
    *
    * This is NOT a complete test of your priority queues. You have to add code
    * to do more testing!
    *
    * You do not have to submit this file, but it won't cause problems if you
    * do.
    */

    #include <cassert>
    #include <iostream>
    #include <ostream>
    #include <stdexcept>
    #include <string>
    #include <vector>
    #include <stdint.h>
    #include <random>
    
    #include "BinaryPQ.hpp"
    #include "Eecs281PQ.hpp"
    #include "PairingPQ.hpp"
    #include "SortedPQ.hpp"
    #include "UnorderedPQ.hpp"

    // A type for representing priority queue types at runtime
    enum class PQType {
        Unordered,
        Sorted,
        Binary,
        Pairing,
    };

    // These can be pretty-printed :)
    std::ostream &operator<<(std::ostream &ost, PQType pqType) {
        switch (pqType) {
        case PQType::Unordered:
            return ost << "Unordered";
        case PQType::Sorted:
            return ost << "Sorted";
        case PQType::Binary:
            return ost << "Binary";
        case PQType::Pairing:
            return ost << "Pairing";
        } // switch

        return ost << "Unknown PQType";
    } // operator<<()


    // Compares two int const* on the integers they point to
    struct IntPtrComp {
        bool operator()(const int *a, const int *b) const {
            return *a < *b;
        } // operator()
    }; // IntPtrComp structure


    // Test the primitive operations on a priority queue:
    // constructor, push, pop, top, size, empty.
    template <template <typename...> typename PQ>
    void testPrimitiveOperations() {
        std::cout << "Testing primitive priority queue operations..." << std::endl;

        PQ<int> pq {};
        Eecs281PQ<int> &eecsPQ = pq;

        eecsPQ.push(3);
        eecsPQ.push(4);
        assert(eecsPQ.size() == 2);
        assert(eecsPQ.top() == 4);

        eecsPQ.pop();
        assert(eecsPQ.size() == 1);
        assert(eecsPQ.top() == 3);
        assert(not eecsPQ.empty());

        eecsPQ.pop();
        assert(eecsPQ.size() == 0);  // NOLINT: Explicit test for size == 0
        assert(eecsPQ.empty());

        //student add more test here
        eecsPQ.push(-5);
        eecsPQ.push(0);
        eecsPQ.push(8);
        eecsPQ.push(8);
        eecsPQ.push(2);

        assert(eecsPQ.size() == 5);
        assert(eecsPQ.top() == 8);

        const std::vector<int> expected {8, 8, 2, 0, -5};

        for (int value : expected) {
            assert(!eecsPQ.empty());
            assert(eecsPQ.top() == value);
            eecsPQ.pop();
        }

        assert(eecsPQ.empty());
        assert(eecsPQ.size() == 0);

        eecsPQ.push(10);
        eecsPQ.push(4);
        assert(eecsPQ.top() == 10);
        eecsPQ.pop();
        assert(eecsPQ.top() == 4);

        eecsPQ.push(12);
        assert(eecsPQ.top() == 12);
        eecsPQ.pop();
        assert(eecsPQ.top() == 4);
        eecsPQ.pop();

        assert(eecsPQ.empty());

        std::cout << "testPrimitiveOperations succeeded!" << std::endl;
    } // testPrimitiveOperations()


    // Test that the priority queue uses its comparator properly. HiddenData
    // can't be compared with operator<, so we use HiddenDataComp{} instead.
    template <template <typename...> typename PQ>
    void testHiddenData() {
        struct HiddenData {
            int data;
        }; // HiddenData structure

        struct HiddenDataComp {
            bool operator()(const HiddenData &a, const HiddenData &b) const {
                // TODO: Finish this comparator
                return a.data < b.data;
            } // operator()()
        }; // comparator

        std::cout << "Testing with hidden data..." << std::endl;

        // TODO: Add code here to actually test with the HiddenData type.
        // Consider writing this code in the style of testPrimitiveOperations
        // above.
        
        PQ<HiddenData, HiddenDataComp> pq{};
        Eecs281PQ<HiddenData, HiddenDataComp> &eecsPQ = pq;
        eecsPQ.push(HiddenData{7});
        eecsPQ.push(HiddenData{2});
        eecsPQ.push(HiddenData{15});
        eecsPQ.push(HiddenData{7});

        assert(eecsPQ.size() == 4);
        assert(eecsPQ.top().data == 15);

        eecsPQ.pop();
        assert(eecsPQ.top().data == 7);

        eecsPQ.pop();
        assert(eecsPQ.top().data == 7);

        eecsPQ.pop();
        assert(eecsPQ.top().data == 2);

        eecsPQ.pop();
        assert(eecsPQ.empty());

        std::cout << "testHiddenData succeeded!" << std::endl;
    } // testHiddenData()


    // Test the last public member function of Eecs281PQ, updatePriorities
    template <template <typename...> typename PQ>
    void testUpdatePriorities() {
        std::vector<int> data {
            1,
            5,  // NOLINT: Some non-trivial number needed here
        };

        PQ<const int *, IntPtrComp> pq {};
        Eecs281PQ<const int *, IntPtrComp> &eecsPQ = pq;

        // NOTE: If you add more data to the vector, don't push the pointers
        //   until AFTER the vector stops changing size! Think about why.
        for (auto &datum : data) {
            eecsPQ.push(&datum);
        } // for

        // Change some element in data (which is pointed to by an element in pq).
        // This new value should be higher than any other so its address will
        // wind qt the top adter updatePriorities.
        auto &datum = data[0];
        datum = 10;  // NOLINT: Some non-trivial number needed here
        eecsPQ.updatePriorities();
        assert(*eecsPQ.top() == 10);
        assert(eecsPQ.top() == &datum);

        // TODO: Add more testing here as you see fit.
        eecsPQ.pop();
        eecsPQ.pop();
        assert(eecsPQ.size() == 0);

        std::vector<int> moreDate {1, 5, 3, 9, 2};

        for (const int& value : moreDate) {
            eecsPQ.push(&value);
        }

        assert(eecsPQ.size() == moreDate.size());
        assert(*eecsPQ.top() == 9);

        moreDate[0] = 20;
        moreDate[3] = -10;

        eecsPQ.updatePriorities();

        assert(eecsPQ.size() == 5);
        assert(*eecsPQ.top() == 20);
        assert(eecsPQ.top() == &moreDate[0]);

        // Check the entire rebuilt priority order.
        const std::vector<int> expected {20, 5, 3, 2, -10};

        for (int value : expected) {
            assert(!eecsPQ.empty());
            assert(*eecsPQ.top() == value);
            eecsPQ.pop();
        }

        assert(eecsPQ.empty());

        // Empty queue must also rebuild safely.
        eecsPQ.updatePriorities();
        assert(eecsPQ.empty());

        std::cout << "testUpdatePriorities succeeded!" << std::endl;

    } // testUpdatePriorities()

    template <template <typename...> typename PQ>
    void testRangeConstructor() {
        const std::vector<int> values {4, 1, 9, 9, -2};

        PQ<int> pq(values.begin(), values.end());

        assert(pq.size() == values.size());

        const std::vector<int> expected {9, 9, 4, 1, -2};

        for (int value : expected) {
            assert(!pq.empty());
            assert(pq.top() == value);
            pq.pop();
        }

        assert(pq.empty());

        // Empty range.
        const std::vector<int> emptyValues;
        PQ<int> emptyPQ(emptyValues.begin(), emptyValues.end());

        assert(emptyPQ.empty());
        assert(emptyPQ.size() == 0);
    }

    // Test the pairing heap's range-based constructor, copy constructor,
    // copy-assignment operator, and destructor
    // TODO: Test other operations specific to this PQ type.
    void testPairing() {
        std::cout << "Testing Pairing Heap separately..." << std::endl;

        // Start a block of code here, so that when it ends, we have
        // a way to know that the destructors are about to be called,
        // but the function hasn't finished yet.
        {
            const std::vector<int> vec { 1, 0, };

            std::cout << "Calling constructors" << std::endl;

            // Range-based constructor
            PairingPQ<int> pairing1 { vec.cbegin(), vec.cend() };

            // Copy constructor
            PairingPQ<int> pairing2 { pairing1 };

            // Copy-assignment operator
            PairingPQ<int> pairing3 {};
            pairing3 = pairing2;

            // A reference to a PairingPQ<T> is a reference to an Eecs281PQ<T>.
            // Yay for polymorphism! We can therefore write:
            Eecs281PQ<int> &pq1 = pairing1;
            Eecs281PQ<int> &pq2 = pairing2;
            Eecs281PQ<int> &pq3 = pairing3;  // NOLINT: Students may add testing that modifies pq3

            pq1.push(3);
            pq2.pop();
            assert(pq1.size() == 3);
            assert(not pq1.empty());
            assert(pq1.top() == 3);
            pq2.push(pq3.top());
            assert(pq2.top() == pq3.top());

            std::cout << "Basic tests done." << std::endl;

            // TODO: Add more code to test addNode, updateElt, etc.

            // Test addNode and updateElt.
            PairingPQ<int> handles;

            auto* node10 = handles.addNode(10);
            auto* node20 = handles.addNode(20);
            auto* node30 = handles.addNode(30);

            assert(handles.size() == 3);
            assert(handles.top() == 30);

            // Update the root to a more extreme value.
            handles.updateElt(node30, 40);
            assert(handles.top() == 40);

            // Update a non-root node to a more extreme value.
            handles.updateElt(node10, 35);
            assert(handles.top() == 40);

            // Update another node beyond the current root.
            handles.updateElt(node20, 50);
            assert(handles.top() == 50);
            assert(handles.size() == 3);

            // Verify complete removal order.
            assert(handles.top() == 50);
            handles.pop();
            assert(handles.top() == 40);
            handles.pop();
            assert(handles.top() == 35);
            handles.pop();
            assert(handles.empty());

            // Test an empty range constructor.
            const std::vector<int> noValues;
            PairingPQ<int> emptyPairing(
                noValues.begin(), noValues.end());

            assert(emptyPairing.empty());

            // Test copy independence.
            PairingPQ<int> original;
            original.addNode(5);
            original.addNode(12);
            original.addNode(8);

            PairingPQ<int> copied(original);
            assert(copied.size() == original.size());
            assert(copied.top() == original.top());

            copied.pop();
            assert(copied.top() == 8);
            assert(original.top() == 12);

            // Test assignment over a nonempty destination.
            PairingPQ<int> assigned;
            assigned.addNode(100);
            assigned = original;

            assert(assigned.size() == original.size());
            assert(assigned.top() == 12);

            assigned.pop();
            assert(assigned.top() == 8);
            assert(original.top() == 12);

            // Test self-assignment.
            assigned = assigned;
            assert(assigned.top() == 8);

            // Test updatePriorities on a populated pairing heap.
            PairingPQ<int> rebuild;
            rebuild.addNode(3);
            rebuild.addNode(7);
            rebuild.addNode(1);

            rebuild.updatePriorities();
            assert(rebuild.size() == 3);
            assert(rebuild.top() == 7);

            rebuild.pop();
            assert(rebuild.top() == 3);

            rebuild.pop();
            assert(rebuild.top() == 1);

            rebuild.pop();
            assert(rebuild.empty());

            rebuild.updatePriorities();
            assert(rebuild.empty());
            
            // That { above creates a scope, and our pairing heaps will fall out
            // of scope at the matching } below.
            std::cout << "Calling destructors" << std::endl;
        } // block for testing destructors

        std::cout << "testPairing succeeded!" << std::endl;
    } // testPairing()


    // Run all tests for a particular PQ type.
    template <template <typename...> typename PQ>
    void testPriorityQueue() {
        testPrimitiveOperations<PQ>();
        testHiddenData<PQ>();
        testUpdatePriorities<PQ>();
        testRangeConstructor<PQ>();
    } // testPriorityQueue()

    // PairingPQ has some extra behavior we need to test in updateElement.
    // This template specialization handles that without changing the nice
    // uniform interface of testPriorityQueue.
    template <>
    void testPriorityQueue<PairingPQ>() {
        testPrimitiveOperations<PairingPQ>();
        testHiddenData<PairingPQ>();
        testUpdatePriorities<PairingPQ>();
        testRangeConstructor<PairingPQ>();
        testPairing();
    } // testPriorityQueue<PairingPQ>()


    int main() {
        const std::vector<PQType> types {
            PQType::Unordered,
            PQType::Sorted,
            PQType::Binary,
            PQType::Pairing,
        };

        std::cout << "PQ tester" << std::endl << std::endl;
        int idx { 0 };
        for (const auto &type : types) {
            std::cout << "  " << idx++ << ") " << type << std::endl;
        } // for
        std::cout << std::endl;

        std::cout << "Select one: ";
        uint32_t choice {};
        std::cin >> choice;
        const PQType pqType = types.at(choice);

        std::cout << "Testing the " << pqType << " PQ...";

        // TODO: Add more cases to test other priority queue types.
        switch (pqType) {
        case PQType::Unordered:
            testPriorityQueue<UnorderedPQ>();
            break;
        case PQType::Sorted:
            testPriorityQueue<SortedPQ>();
            break;
        case PQType::Binary:
            testPriorityQueue<BinaryPQ>();
            break;
        case PQType::Pairing:
            testPriorityQueue<PairingPQ>();
            break;
        default:
            std::cout << "Unrecognized PQ type " << pqType << " in main.\n"
                    << "You must add tests for all PQ types." << std::endl;
            return 1;
        } // switch

        std::cout << "All tests succeeded!" << std::endl;

        return 0;
    } // main()
