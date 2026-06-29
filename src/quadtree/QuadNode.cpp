#include "quadtree/QuadNode.hpp"

namespace qt {

QuadNode::QuadNode(const Rectangle& bounds, int capacity, int maxDepth, int depth)
    : bounds_(bounds), capacity_(capacity), maxDepth_(maxDepth), depth_(depth) {
    particles_.reserve(static_cast<size_t>(capacity) + 1);
}

bool QuadNode::insert(const Particle* p, int64_t& comparisons) {
    // 1) ¿La partícula cae dentro de la región de este nodo?
    ++comparisons;
    if (!bounds_.contains(*p)) {
        return false;
    }

    // 2) Si es hoja y hay espacio (o ya llegamos a la profundidad máxima),
    //    se guarda aquí.
    if (isLeaf()) {
        if (static_cast<int>(particles_.size()) < capacity_ || depth_ >= maxDepth_) {
            particles_.push_back(p);
            return true;
        }
        // 3) Está llena: subdividir y reubicar las partículas existentes.
        subdivide(comparisons);
    }

    // 4) Delegar al hijo correspondiente.
    if (nw_->insert(p, comparisons)) return true;
    if (ne_->insert(p, comparisons)) return true;
    if (sw_->insert(p, comparisons)) return true;
    if (se_->insert(p, comparisons)) return true;

    // No debería ocurrir si la partícula está dentro de bounds_, pero por
    // seguridad (casos de borde con coordenadas exactas) la guardamos aquí.
    particles_.push_back(p);
    return true;
}

void QuadNode::subdivide(int64_t& comparisons) {
    double hw = bounds_.w * 0.5;
    double hh = bounds_.h * 0.5;
    double x = bounds_.x, y = bounds_.y;
    int d = depth_ + 1;

    nw_ = std::make_unique<QuadNode>(Rectangle(x,      y,      hw, hh), capacity_, maxDepth_, d);
    ne_ = std::make_unique<QuadNode>(Rectangle(x + hw, y,      hw, hh), capacity_, maxDepth_, d);
    sw_ = std::make_unique<QuadNode>(Rectangle(x,      y + hh, hw, hh), capacity_, maxDepth_, d);
    se_ = std::make_unique<QuadNode>(Rectangle(x + hw, y + hh, hw, hh), capacity_, maxDepth_, d);

    // Reubicar las partículas que estaban en esta hoja.
    for (const Particle* p : particles_) {
        if (nw_->insert(p, comparisons)) continue;
        if (ne_->insert(p, comparisons)) continue;
        if (sw_->insert(p, comparisons)) continue;
        se_->insert(p, comparisons);
    }
    particles_.clear();
    particles_.shrink_to_fit();
}

void QuadNode::queryRange(const Rectangle& range,
                          std::vector<const Particle*>& out,
                          int64_t& comparisons) const {
    // Poda: si la región no toca este nodo, no bajamos por aquí.
    ++comparisons;
    if (!bounds_.intersects(range)) return;

    if (isLeaf()) {
        for (const Particle* p : particles_) {
            ++comparisons;
            if (range.contains(*p)) out.push_back(p);
        }
        return;
    }

    nw_->queryRange(range, out, comparisons);
    ne_->queryRange(range, out, comparisons);
    sw_->queryRange(range, out, comparisons);
    se_->queryRange(range, out, comparisons);
}

void QuadNode::queryCircle(const Circle& circle,
                           std::vector<const Particle*>& out,
                           int64_t& comparisons) const {
    // Poda: si el círculo no toca este nodo, descartamos la rama entera.
    ++comparisons;
    if (!circle.intersects(bounds_)) return;

    if (isLeaf()) {
        for (const Particle* p : particles_) {
            ++comparisons;
            if (circle.contains(p->x, p->y)) out.push_back(p);
        }
        return;
    }

    nw_->queryCircle(circle, out, comparisons);
    ne_->queryCircle(circle, out, comparisons);
    sw_->queryCircle(circle, out, comparisons);
    se_->queryCircle(circle, out, comparisons);
}

void QuadNode::collectBounds(std::vector<Rectangle>& out) const {
    out.push_back(bounds_);
    if (!isLeaf()) {
        nw_->collectBounds(out);
        ne_->collectBounds(out);
        sw_->collectBounds(out);
        se_->collectBounds(out);
    }
}

int QuadNode::countNodes() const {
    if (isLeaf()) return 1;
    return 1 + nw_->countNodes() + ne_->countNodes()
             + sw_->countNodes() + se_->countNodes();
}

} // namespace qt
