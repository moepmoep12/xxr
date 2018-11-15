#pragma once

#include <memory>
#include <unordered_map>
#include <cstdint>

namespace XCS
{

    template <typename T, typename Action, class Symbol, class Condition, class Classifier, class Constants, class ClassifierPtrSet, class Population>
    class MatchSet : public ClassifierPtrSet
    {
    protected:
        using ClassifierPtr = std::shared_ptr<Classifier>;
        using ClassifierPtrSet::m_constants;
        using ClassifierPtrSet::m_availableActions;
        using ClassifierPtrSet::m_set;

        // GENERATE COVERING CLASSIFIER
        virtual ClassifierPtr generateCoveringClassifier(const std::vector<T> & situation, const std::unordered_set<Action> & unselectedActions, uint64_t timeStamp) const
        {
            auto cl = std::make_shared<Classifier>(situation, Random::chooseFrom(unselectedActions), timeStamp, m_constants);
            cl->condition.randomGeneralize(m_constants.generalizeProbability);

            return cl;
        }

    public:
        // Constructor
        using ClassifierPtrSet::ClassifierPtrSet;

        MatchSet(Population & population, const std::vector<T> & situation, uint64_t timeStamp, const Constants & constants, const std::unordered_set<Action> & availableActions) :
            ClassifierPtrSet(constants, availableActions)
        {
            regenerate(population, situation, timeStamp);
        }

        // Destructor
        virtual ~MatchSet() = default;

        // GENERATE MATCH SET
        virtual void regenerate(Population & population, const std::vector<T> & situation, uint64_t timeStamp)
        {
            // Set theta_mna (the minimal number of actions) to the number of action choices if theta_mna is 0
            auto thetaMna = (m_constants.thetaMna == 0) ? m_availableActions.size() : m_constants.thetaMna;

            auto unselectedActions = m_availableActions;

            m_set.clear();

            while (m_set.empty())
            {
                for (auto && cl : population)
                {
                    if (cl->condition.matches(situation))
                    {
                        m_set.insert(cl);
                        unselectedActions.erase(cl->action);
                    }
                }

                // Generate classifiers covering the unselected actions
                if (m_availableActions.size() - unselectedActions.size() < thetaMna)
                {
                    auto coveringClassifier = generateCoveringClassifier(situation, unselectedActions, timeStamp);
                    if (!coveringClassifier->condition.matches(situation))
                    {
                        std::cerr <<
                            "Error: The covering classifier does not contain the current situation!\n"
                            "  - Current situation: ";
                        for (auto && s : situation)
                        {
                            std::cerr << s << " ";
                        }
                        std::cerr << "\n  - Covering classifier: " << *coveringClassifier << "\n" << std::endl;
                        assert(false);
                    }
                    population.insert(coveringClassifier);
                    population.deleteExtraClassifiers();
                    m_set.clear();
                }
            }
        }
    };

}