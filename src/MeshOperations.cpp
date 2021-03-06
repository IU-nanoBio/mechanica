/*
 * MeshOperations.cpp
 *
 *  Created on: Nov 20, 2017
 *      Author: andy
 */

#include "MeshOperations.h"
#include "MxMesh.h"
#include <algorithm>
#include <limits>
#include <iostream>



/**
 * The std *_heap functions produce a max heap. We need the lowest energy items
 * first, so flip the compare op here.
 */
struct MeshOperationComp
{
    bool operator()(const MeshOperation* a, const MeshOperation *b) const {
        return a->energy() > b->energy();
    };
};

MeshOperations::MeshOperations(MeshPtr _mesh, float _shortCutoff,
        float _longCutoff) :
    mesh{_mesh}, shortCutoff{_shortCutoff}, longCutoff{_longCutoff}
{
}

HRESULT MeshOperations::positionsChanged(TriangleContainer::const_iterator triBegin,
        TriangleContainer::const_iterator triEnd) {

#ifndef NDEBUG
    if(shouldStop) return S_OK;
#endif

    for(auto iter = triBegin; iter != triEnd; ++iter) {
        const PolygonPtr tri = *iter;
        int minIndx = -1, maxIndx = -1;
        float minEdge = std::numeric_limits<float>::max();
        float maxEdge = std::numeric_limits<float>::min();
        for(int i = 0; i < 3; ++i) {
            float dist = Magnum::Math::distance(tri->vertices[i]->position, tri->vertices[(i+1)%3]->position);
            if(dist < minEdge) {
                minEdge = dist;
                minIndx = i;
            }
            if(dist > maxEdge) {
                maxEdge = dist;
                maxIndx = i;
            }
        }


    }

    return S_OK;
}

HRESULT MeshOperations::valenceChanged(const VertexPtr vert) {
    MeshOperation *meshOp;


    return S_OK;
}

void setMeshOpDebugMode(uint c) {

}

HRESULT MeshOperations::removeDependentOperations(const PolygonPtr tri) {
    Container::iterator iter = c.begin();
    bool found = false;
    while((iter = findDependentOperation(iter, tri)) != c.end()) {
        MeshOperation *op = *iter;
        delete op;
        c.erase(iter++);
        found = true;
    }
    // stuff was removed, need to re-order the heap.
    if(found) {
        std::make_heap(c.begin(), c.end(), MeshOperationComp{});
    }
    return S_OK;
}

HRESULT MeshOperations::removeDependentOperations(const VertexPtr vert) {
    Container::iterator iter = c.begin();
    bool found = false;
    while((iter = findDependentOperation(iter, vert)) != c.end()) {
        MeshOperation *op = *iter;
        iter = c.erase(iter);
        delete op;
        found = true;
    }
    // stuff was removed, need to re-order the heap.
    if(found) {
        std::make_heap(c.begin(), c.end(), MeshOperationComp{});
    }
    return S_OK;
}


/**
 * Process ALL pending operations in the queue.
 *
 * Note, it is perfectly OK if an operation fails. This can occur if previous
 * operations changed the mesh geometry or topology, and a pending operation
 * may no longer be valid. For example, an edge collapse moves triangle vertices,
 * but needs to make sure that any moved triangles don't become inverted. The
 * original triangle positions might have safely allowed an operation, but a
 * previous operation might have moved the triangle position, so now,
 * when the pending operation is attempted, it could invert a triangle. So, the
 * operation fails, gets removed from the queue, and we continue processing any
 * remaining ops.
 */
HRESULT MeshOperations::apply() {

    HRESULT res = S_OK;

    if(debugMode) return S_OK;

    while((currentOp = pop()) != nullptr) {
#ifndef NDEBUG
        if(!shouldStop)  res = currentOp->apply();
#else
        res = currentOp->apply();
#endif
        mesh->positionsChanged();
        // TODO, log the failed operation somehow.
        delete currentOp;
    }

    assert(currentOp == nullptr);

    return S_OK;
}


void MeshOperations::push(MeshOperation* x) {
    c.push_back(x);
    std::push_heap(c.begin(), c.end(), MeshOperationComp{});
}

MeshOperation* MeshOperations::pop() {
    if(c.size() > 0) {
        // moves the largest to the back
        std::pop_heap(c.begin(), c.end(), MeshOperationComp{});
        MeshOperation *result = c.back();
        c.pop_back();
        return result;
    }
    return nullptr;
}

void MeshOperations::setShortCutoff(float val) {
    shortCutoff = val;
}

void MeshOperations::setLongCutoff(float val) {
    longCutoff = val;
}

MeshOperations::Container::iterator MeshOperations::findDependentOperation(
        MeshOperations::Container::iterator start, const PolygonPtr tri) {
    return std::find_if(start, c.end(),
            [tri](const MeshOperation* op)->bool { return op->depends(tri); });
}

MeshOperations::Container::iterator MeshOperations::findDependentOperation(
        MeshOperations::Container::iterator start, const VertexPtr vert) {
    return std::find_if(start, c.end(),
            [vert](const MeshOperation* op)->bool { return op->depends(vert); });
}



MeshOperation* MeshOperations::findMatchingOperation(CVertexPtr vertex)  {
    Container::iterator iter = std::find_if(c.begin(), c.end(),
                                            [vertex](const MeshOperation* op)->bool { return op->equals(vertex); });
    if(iter != c.end()) {
        return *iter;
    }
    else if (currentOp) {
        return currentOp->equals(vertex) ? currentOp : nullptr;
    }
    else {
        return nullptr;
    }
}

MeshOperations::~MeshOperations() {
}



void MeshOperations::setDebugMode(bool val) {
    debugMode = val;
    std::cout << "setting debug mode: " << debugMode << std::endl;
    if(pendingDebugOp) {
        pendingDebugOp->apply();
        delete pendingDebugOp;
        pendingDebugOp = nullptr;
    }
    if(debugMode) {
        pendingDebugOp = pop();
        if(pendingDebugOp) pendingDebugOp->mark();
    }
}

HRESULT MeshOperations::debugStep() {

    MeshOperation *op;
    HRESULT res = S_OK;

    if(pendingDebugOp) {
        std::cout << "debug step " << debugCnt++ << std::endl;
        res = pendingDebugOp->apply();
        delete pendingDebugOp;
    }

    pendingDebugOp = pop();
    if(pendingDebugOp) pendingDebugOp->mark();

    return res;
}

MeshOperation::MeshOperation(MeshPtr _mesh)  :
    mesh{_mesh}, ops{&_mesh->meshOperations} {
}


