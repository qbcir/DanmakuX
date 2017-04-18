#include "PhyTree.h"
#include "../Data/JSONParser.h"
#include <limits>

float ComputeDistance(float r1, float r2,
                      float a1, float a2,
                      float annular_shift) {

    return std::sqrt((r2-r1)*(r2-r1)+
        2*(r1+annular_shift)*(r2+annular_shift)*(1-std::cos(a1 - a2)));
}

PhyTreeNode* PhyTreeNode::createFromJson(PhyTree* tree, int depth, const rapidjson::Value& el) {
    auto ptn = new (std::nothrow) PhyTreeNode();
    if (ptn && ptn->initWithJson(tree, depth, el)) {
        ptn->autorelease();
        return ptn;
    }
    CC_SAFE_DELETE(ptn);
    return nullptr;
}

bool PhyTreeNode::initWithJson(PhyTree* tree, int depth, const rapidjson::Value &el) {
    this->setDepth(depth);
    this->setTree(tree);
    tree->addNode(this);
    auto member_end = el.MemberEnd();
    auto it = el.FindMember("children");
    if (it != member_end) {
        auto& children = it->value;
        for (auto ch_it = children.Begin();
             ch_it != children.End();
             ++ch_it) {
            auto ch_ptn = PhyTreeNode::createFromJson(tree, depth + 1, *ch_it);
            if (!ch_ptn) {
                return false;
            }
            this->addChildNode(ch_ptn);
            ch_ptn->setParentNode(this);
            cocos2d::log("depth: %d", m_depth + 1);
        }
    }
    auto tmp = cocos2d::Label::create();
    tmp->setString("*");
    tmp->setPosition(0, 0);
    this->addChild(tmp);
    return true;
}

void PhyTree::addNode(PhyTreeNode* node) {
    CCASSERT(node != nullptr, "tree node is null");
    m_nodes.pushBack(node);
}

PhyTree* PhyTree::createFromJson(const std::string &fname) {
    auto pt = new (std::nothrow) PhyTree();
    if (pt && pt->initWithJson(fname)) {
        pt->autorelease();
        return pt;
    }
    CC_SAFE_DELETE(pt);
    return nullptr;
}

bool PhyTree::initWithJson(const std::string &fname) {
    rapidjson::Document doc;
    if (!parseJsonDocument(doc, fname)) {
        return false;
    }
    auto el = static_cast<rapidjson::Value*>(&doc);
    auto member_end = el->MemberEnd();
    auto it = el->FindMember("root");
    if (member_end == it) {
        return false;
    }
    auto root_node = PhyTreeNode::createFromJson(this, 1, it->value);
    if (!root_node) {
        return false;
    }
    this->placeNodes();
    for (auto node: m_nodes) {
        auto pos = node->getPosition();
        this->addChild(node);
        cocos2d::log("pos: %f %f", pos.x, pos.y);
    }
    return true;
}

void PhyTreeNode::setTree(PhyTree* tree) {
    m_tree = tree;
}

float PhyTreeNode::getBubbleSize() const {
    return 0;
}

float PhyTreeNode::getNodeSpan() const {
    return 12;
}

PhyTreeNode* PhyTreeNode::getParentNode() {
    return m_parentNode;
}

void PhyTreeNode::setParentNode(PhyTreeNode* node) {
    m_parentNode = node;
}

cocos2d::Vector<PhyTreeNode *>& PhyTreeNode::getChildrenNodes() {
    return m_children;
}

bool PhyTreeNode::isLeafNode() const {
    return m_children.size() == 0;
}

int PhyTreeNode::getDepth() const {
    return m_depth;
}

void PhyTreeNode::setDepth(int depth) {
    m_depth = depth;
}

float PhyTreeNode::getAngle() const {
    return m_angle;
}

void PhyTreeNode::setAngle(float angle) {
    m_angle = angle;
}

float PhyTreeNode::getRadius() const {
    return m_radius;
}

void PhyTreeNode::setRadius(float radius) {

}

void PhyTreeNode::addChildNode(PhyTreeNode *node) {
    CCASSERT(node != nullptr, "tree node is null");
    m_children.pushBack(node);
}

void PhyTree::setRadial(bool radial) {
    m_radial = radial;
}

bool PhyTree::isRadial() const {
    return m_radial;
}

bool PhyTree::atLeastOneDimensionFixed() const {
    return m_topBottomSpacing != Spacing::FixedStep ||
           m_leftRightSpacing != Spacing::FixedStep;
}

void PhyTree::setupTopBottomSpacing() {
    if (m_topBottomSpacing == Spacing::FixedStep) {
        m_offsets.y = std::max(m_fontSize, -m_extents[1][0] * m_fixedWidth.x);
        m_size.x = m_extents[0][1] * m_fixedWidth.x;
        m_scales.x = m_fixedWidth.x;
    } else {
        m_scales.x = (m_size.x - 0.0f/* pad_height() */) / m_extents[0][1];
    }
}

void PhyTree::setupLeftRightSpacing() {
    if (m_radial && atLeastOneDimensionFixed()) {
        m_offsets.y = 0;
    }
    if (m_leftRightSpacing == Spacing::FixedStep) {
        m_size.y = m_maxDepth * m_fixedWidth.y;
        m_scales.y = (m_size.y - m_offsets.y - m_leftOffset) / m_extents[1][1];
    } else {
        float available_width = m_size.y - m_offsets.y - m_leftOffset;
        if (available_width * 0.5f < m_labelWidth) {
            m_shownFontSize *= (available_width * 0.5 / m_labelWidth);
            m_labelWidth = available_width * 0.5f;
        }
        m_scales.y = (m_size.y - m_offsets.y - m_leftOffset - m_labelWidth) / m_extents[1][1];
    }
}

float PhyTree::separation(PhyTreeNode* node1, PhyTreeNode* node2) {
    return 0;
}

void PhyTree::processInternalNode(PhyTreeNode* node) {
    std::vector<float> xs;
    auto& children = node->getChildrenNodes();
    float tl = 0.0f;
    for (PhyTreeNode* ch_node : children) {
        tl += this->treeLayout(ch_node);
    }
    tl /= children.size();
    cocos2d::log("tl=%f", tl);
    node->setPositionX(tl);
}

float PhyTree::treeLayout(PhyTreeNode* node) {
    auto parentNode = node->getParentNode();
    if (parentNode) {
        if (m_doScaling) {
            node->setPositionY(/* branch length */50.0f);
            node->setPositionY(node->getPositionY() + parentNode->getPositionY());
        } else {
            node->setPositionY(node->getDepth());
        }
    } else {
        m_x = 0.0f;
        m_extents = {{{{0.0f, 0.0f}}, {{0.0f, 0.0f}}}};
        m_lastNode = nullptr;
        m_lastSpan = 0.0f;
        node->setPositionY(0.0f);
    }
    if (node->isLeafNode()) {
        float node_span = node->getNodeSpan() / m_rescaleNodeSpan;
        m_x = m_x + separation(m_lastNode, node) + m_lastSpan + node_span;
        cocos2d::log("m_x=%f", m_x);
        node->setPositionX(m_x);
        m_extents[1][1] = std::max(m_extents[1][1], node->getPositionY());
        m_extents[1][0] = std::min(m_extents[1][0], node->getPositionY() - node_span * 0.5f);
        if (m_underCollapsedParent) {
            m_extents[0][1] = std::max(m_extents[0][1], m_saveX
                    + (node->getPositionX() - m_saveX) * m_compression
                    + m_saveSpan
                    + (node_span * 0.5f + separation(m_lastNode, node)) * m_compression);
        } else {
            m_extents[0][1] = std::max(m_extents[0][1], m_x
                    + node_span * 0.5f
                    + separation(m_lastNode, node));
        }
        m_lastNode = node;
        m_lastSpan = node_span;
    } else {
        if (/* m_underCollapsedParent*/ false) {
            m_saveX = m_x;
            m_saveSpan = m_lastSpan * 0.5f;
            m_underCollapsedParent = true;
            processInternalNode(node);
            m_underCollapsedParent = false;
        } else {
            processInternalNode(node);
        }
    }
    return node->getPositionX();
}

void PhyTree::placeNodes() {
    m_rescaleNodeSpan = std::numeric_limits<float>::max();
    for (PhyTreeNode* node : m_nodes) {
        if (node->getNodeSpan() < m_rescaleNodeSpan) {
            m_rescaleNodeSpan = node->getNodeSpan();
        }
    }
    m_nodes.front()->setPositionX(treeLayout(m_nodes.front()));
    m_maxDepth = std::numeric_limits<float>::min();
    for (PhyTreeNode* node : m_nodes) {
        if (node->getDepth() > m_maxDepth) {
            m_maxDepth = node->getDepth();
        }
    }
    setupTopBottomSpacing();
    m_shownFontSize = std::min(m_fontSize, m_scales.x);
    if (m_radial) {
        placeNodesRadial();
    } else {
        for(PhyTreeNode* node : m_nodes) {
            auto node_pos = node->getPosition();
            node_pos.x *= m_scales.x;
            node_pos.y *= m_scales.y;
            node->setPosition(node_pos);
        }
    }
}

void PhyTree::placeNodesRadial() {
    float effective_span = m_extents[0][1] * m_scales.x;
    float min_r = 0;
    float max_r = 0;
    float circ_position;
    for (PhyTreeNode* node : m_nodes) {
        auto node_pos = node->getPosition();
        circ_position = node_pos.x * m_scales.x;
        node->setAngle(2 * M_PI * circ_position / effective_span);
        node->setRadius(node_pos.y * m_scales.y  / m_size.y);
        max_r = std::max(max_r, node->getRadius());
    }
    setupLeftRightSpacing();
    float annular_shift = 0;
    float last_circ_position = 0;
    PhyTreeNode* last_node = nullptr;
    for (PhyTreeNode* node : m_nodes) {
        auto node_pos = node->getPosition();
        circ_position = node_pos.x * m_scales.x;
        if (!node->getChildren().empty()) continue;
        if (last_node) {
            float required_spacing = circ_position - last_circ_position;
            float radial_dist = ComputeDistance(node->getRadius(), last_node->getRadius(),
                node->getAngle(), last_node->getAngle(), annular_shift);
            float local_mr = radial_dist > 0 ? (required_spacing / radial_dist) :
                (10 * m_maxRadius);
            if (local_mr > m_maxRadius) {
                float dd = required_spacing / m_maxRadius;
                float b = node->getRadius() + last_node->getRadius();
                float tmp1 = last_node->getRadius() - node->getRadius();
                float tmp2 = last_node->getAngle() - node->getAngle();
                float c = b-((dd*dd-tmp1*tmp1)/2*(1-std::cos(tmp2)));
                float st = std::sqrt(b*b-4*c);
                annular_shift = std::min(m_annularLimit * max_r, (st - b) / 2);
                min_r = m_maxRadius;
            } else {
                min_r = local_mr;
            }
        }
        last_node = node;
        last_circ_position = circ_position;
    }
    float radius = std::min(m_maxRadius, std::max(effective_span/(2*float(M_PI)), min_r));
    if (annular_shift) {
        float scaler = 1;
        for (PhyTreeNode* node : m_nodes) {
            auto node_pos = node->getPosition();
            node->setRadius(node_pos.y*m_scales.y/m_size.y + annular_shift);
            scaler = std::max(scaler, node->getRadius());
        }
        if (scaler > 1) {
            m_scales.x /= scaler;
            m_scales.y /= scaler;
            annular_shift /= scaler;
        }
    }
    if (atLeastOneDimensionFixed()) {
        radius = std::min(radius, std::min(effective_span, m_extents[1][1]*m_scales.y) * 0.5f - radius*annular_shift);
    }
    float radial_center = radius;
    float radius_pad_for_bubbles = radius;
    for (PhyTreeNode* node : m_nodes) {
        this->cartesianToPolar(node, radius, annular_shift);
        float nr = node->getRadius();
        if (m_drawSizeBubbles) {
            nr += node->getBubbleSize();
        }
        radius_pad_for_bubbles = std::max(radius_pad_for_bubbles, nr);
    }
    m_size.x = radial_center + radius;
    m_size.y = radial_center + radius;
}

void PhyTree::cartesianToPolar(PhyTreeNode* node, float radius, float annular_shift) {
    auto pos = node->getPosition();
    pos.x *= m_scales.x;
    pos.y *= m_scales.y;
    float radial_center = (radius * pos.y / m_size.y + annular_shift);
    float angle = 2*float(M_PI)*pos.x*m_scales.x / m_size.x;
    pos.x = radial_center * std::sin(angle);
    pos.y = radial_center * std::cos(angle);
    node->setPosition(pos);
    node->setAngle(angle);
    node->setRadius(radial_center);
}

float PhyTree::getSpacingX() const {
    return m_fixedWidth.x;
}

float PhyTree::getSpacingY() const {
    return m_fixedWidth.y;
}

const cocos2d::Vec2& PhyTree::getSpacing() const {
    return m_fixedWidth;
}

void PhyTree::setSpacingX(float s, bool skipRender) {
    m_fixedWidth.x = s;
    if (!skipRender) {
        placeNodes();
    }
}

void PhyTree::setSpacingY(float s, bool skipRender) {
    m_fixedWidth.y = s;
    if (!skipRender) {
        placeNodes();
    }
}

void PhyTree::setSpacing(const cocos2d::Vec2& s, bool skipRender) {
    m_fixedWidth = s;
    if (!skipRender) {
        placeNodes();
    }
}
