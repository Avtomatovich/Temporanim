#include "animautomaton.h"

AnimAutomaton::AnimAutomaton(const Model &model,
                             Animator &animator)
    : m_model(model), m_animator(animator)
{}

void AnimAutomaton::update(float dt) {
    if (m_state == AnimState::HIT && m_animator.atEnd()) {
        for (const std::string& animName: {"idle", "stand"}) {
            if (m_animator.swap(animName, true)) {
                m_state = AnimState::IDLE;
                break;
            }
        }
    }
}

void AnimAutomaton::onHit() {
    if (m_state == AnimState::IDLE) {
        m_state = AnimState::HIT;

        for (const std::string& animName: {"impact", "hit", "strike"}) {
            if (m_animator.swap(animName, false)) break;
        }
    } else {
        m_animator.reset();
    }
}
