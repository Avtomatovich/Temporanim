#include "animator.h"
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <stdexcept>
#include <algorithm>

Animator::Animator(const AnimData& animData) :
    m_anims(animData.animations),
    m_skeleton(animData.skeleton),
    m_boneMap(animData.boneToIdx),
    m_anim(m_anims.empty() ? nullptr : std::make_unique<Animation>(m_anims.front())),
    m_animIter(m_anims.begin())
{}

const std::vector<glm::mat4>& Animator::getSkinMats() const {
    return m_skinMats;
}

const bool Animator::hasAnim() const {
    return !m_anims.empty();
}

void Animator::play() {
    m_isPlaying = !m_isPlaying;
}

void Animator::reset() {
    // Reset ticks
    m_ticks = 0.f;

    // Set to play
    m_isPlaying = true;
}

void Animator::swapForward() {
    // Return if empty
    if (m_anims.empty()) return;

    // Shift iterator forward
    ++m_animIter;

    // Loop to beginning if iterator reaches end
    if (m_animIter == m_anims.end()) m_animIter = m_anims.begin();

    // Fetch next animation
    m_anim = std::make_unique<Animation>(*m_animIter);

    reset();
}

void Animator::swapBackward() {
    // Return if empty
    if (m_anims.empty()) return;

    // Shift iterator back
    --m_animIter;

    // Loop to end if iterator reaches beginning
    if (m_animIter < m_anims.begin()) m_animIter = m_anims.end() - 1;

    // Fetch previous animation
    m_anim = std::make_unique<Animation>(*m_animIter);

    reset();
}

void Animator::update(float deltaTime) {
    // Do not update if no animation or not playing
    if (!m_anim || !m_isPlaying) return;

    // Update number of ticks
    m_ticks += deltaTime * m_anim->ticksPerSec;

    // Clamp number of ticks
    m_ticks = fmod(m_ticks, m_anim->duration);

    // Compute skinning matrices
    computeSkinMats(m_ticks);
}

void Animator::computeSkinMats(float now) {
    // Reset bone CTMs
    for (glm::mat4& skinMat : m_skinMats) skinMat = glm::mat4{1.f};

    // Init bone CTMs at current num of ticks
    computeBoneMats(now);

    // Compute bone CTMs by recursing on bones with no parents
    for (const Bone& bone : m_skeleton) {
        if (bone.parent < 0) {
            processBone(bone.name, glm::mat4{1.f});
        }
    }

    // Compute skinning matrices
    for (int i = 0; i < m_skinMats.size(); ++i) {
        // Apply offset (inverse bind pose) matrix to bone CTM
        m_skinMats[i] *= m_skeleton[i].offset;
    }
}

void Animator::processBone(const std::string& name, glm::mat4 ctm) {
    // Fetch skeleton index of bone
    int i = m_boneMap.at(name);

    // Apply transform to ctm
    ctm *= m_skinMats[i];

    // Set bone ctm to current ctm
    m_skinMats[i] = ctm;

    // Iterate over skeleton indexes of child bones
    for (int child : m_skeleton[i].children) {
        // Recurse on child bone
        processBone(m_skeleton[child].name, ctm);
    }
}

// // COMPUTING BONE CTMs
void Animator::computeBoneMats(float now) {
    for (const BoneAnim& boneAnim : m_anim->boneAnims) {
        // Fetch skeleton index of bone
        int i = m_boneMap.at(boneAnim.boneName);

        m_skinMats[i] = lerper(boneAnim.positions, now) *
                        lerper(boneAnim.rotations, now) *
                        lerper(boneAnim.scalings, now);
    }
}

int Animator::getNextIdx(const auto& keyframes, float now) {
    // Throw exception if empty
    if (keyframes.empty()) throw std::runtime_error("No keyframes");

    // Clamp current time between first and last keyframe timestamps
    now = std::clamp(now, keyframes.at(0).time, keyframes.at(keyframes.size() - 1).time);

    for (int i = 1; i < keyframes.size(); ++i) {
        // Return index as soon as current time is passed
        if (now < keyframes.at(i).time) {
            return i;
        }
    }

    return keyframes.size() - 1;
}

glm::mat4 Animator::lerper(const auto& keyframes, float now) {
    // Return static pose if only one keyframe
    if (keyframes.size() == 1) {
        switch(keyframes.at(0).type) {
            case TransformationType::TRANSFORMATION_TRANSLATE:
                return glm::translate(keyframes.at(0).pos.value());
            case TransformationType::TRANSFORMATION_ROTATE:
                return glm::toMat4(keyframes.at(0).rot.value());
            case TransformationType::TRANSFORMATION_SCALE:
                return glm::scale(keyframes.at(0).scale.value());
            default:
                return glm::mat4{1.f};
        }
    }

    // Get previous and next keyframes
    int idx = getNextIdx(keyframes, now);
    const Keyframe& prev = keyframes.at(idx - 1);
    const Keyframe& next = keyframes.at(idx);
    float t = (now - prev.time) / (next.time - prev.time);

    switch(prev.type) {
        case TransformationType::TRANSFORMATION_TRANSLATE:
            if (!prev.pos.has_value() || !next.pos.has_value()) {
                throw std::invalid_argument("Position keyframe is null");
            }
            return glm::translate(glm::mix(prev.pos.value(), next.pos.value(), t));
        case TransformationType::TRANSFORMATION_ROTATE:
            if (!prev.rot.has_value() || !next.rot.has_value()) {
                throw std::invalid_argument("Rotation keyframe is null");
            }
            return glm::toMat4(glm::normalize(glm::slerp(prev.rot.value(), next.rot.value(), t)));
        case TransformationType::TRANSFORMATION_SCALE:
            if (!prev.scale.has_value() || !next.scale.has_value()) {
                throw std::invalid_argument("Scaling keyframe is null");
            }
            return glm::scale(glm::mix(prev.scale.value(), next.scale.value(), t));
        default:
            throw std::invalid_argument("Invalid keyframe type");
    }
}
