#ifndef ANIMAUTOMATON_H
#define ANIMAUTOMATON_H

#include "geometry/model.h"
#include "animator.h"

enum class AnimState {
    IDLE,
    HIT
};

class AnimAutomaton
{
public:
    AnimAutomaton(const Model& model,
                  Animator& animator);

    void update(float dt);

    void onHit();

private:
    const Model& m_model;
    Animator& m_animator;

    AnimState m_state = AnimState::IDLE;
};

#endif // ANIMAUTOMATON_H
