#include "quadtree/QuadTree.hpp"

namespace qt {

QuadTree::QuadTree(const Rectangle& bounds, int capacity, int maxDepth)
    : bounds_(bounds), capacity_(capacity), maxDepth_(maxDepth) {
    root_ = std::make_unique<QuadNode>(bounds_, capacity_, maxDepth_);
}

int64_t QuadTree::build(const std::vector<Particle>& particles) {
    // Reconstrucción total: crear raíz nueva e insertar todo.
    root_ = std::make_unique<QuadNode>(bounds_, capacity_, maxDepth_);
    int64_t comparisons = 0;
    for (const Particle& p : particles) {
        root_->insert(&p, comparisons);
    }
    return comparisons;
}

void QuadTree::clear() {
    root_ = std::make_unique<QuadNode>(bounds_, capacity_, maxDepth_);
}

std::vector<const Particle*> QuadTree::queryRange(const Rectangle& range,
                                                  int64_t& comparisons) const {
    std::vector<const Particle*> out;
    if (root_) root_->queryRange(range, out, comparisons);
    return out;
}

std::vector<const Particle*> QuadTree::queryNeighbors(double x, double y,
                                                      double radius,
                                                      int64_t& comparisons) const {
    std::vector<const Particle*> out;
    if (root_) {
        Circle circle(x, y, radius);
        root_->queryCircle(circle, out, comparisons);
    }
    return out;
}

std::vector<Rectangle> QuadTree::allBounds() const {
    std::vector<Rectangle> out;
    if (root_) root_->collectBounds(out);
    return out;
}

int QuadTree::nodeCount() const {
    return root_ ? root_->countNodes() : 0;
}

} // namespace qt
