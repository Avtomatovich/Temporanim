#ifndef ANIMATOR_H
#define ANIMATOR_H

#include <memory>
#include "utils/sceneparser.h"

class Animator
{
public:
    Animator(const AnimData& animData);

    const std::vector<glm::mat4>& getSkinMats() const;

    const bool hasAnim() const;

    void update(float deltaTime);

    void play();

    void swap(bool toNext);

private:
    const std::vector<Animation>& m_anims;
    const std::vector<Bone>& m_skeleton;
    const std::unordered_map<std::string, int>& m_boneMap;
    std::unique_ptr<Animation> m_anim;
    std::vector<Animation>::const_iterator m_animIter;

    float m_ticks = 0.f;
    std::vector<glm::mat4> m_skinMats{m_skeleton.size(), glm::mat4{1.f}};
    bool m_isPlaying = true;

    void computeSkinMats(float now);
    void computeBoneMats(float now);

    void processBone(const std::string& name, glm::mat4 mat);

    int getNextIdx(const auto& keyframes, float now);
    glm::mat4 lerper(const auto& keyframes, float now);

    void reset();
};

#endif // ANIMATOR_H
