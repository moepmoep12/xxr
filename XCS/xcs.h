#pragma once

#include <algorithm>
#include <vector>
#include <deque>
#include <unordered_set>
#include <cstdint>
#include <cstddef>

#include "xcs_constants.h"
#include "symbol.h"
#include "classifier.h"
#include "random.h"

template <class S = BinarySymbol, typename Action = int>
class XCS
{
protected:
	std::deque<Classifier<S, Action>> m_population; // [P]

	XCSConstants m_constants;

	std::unordered_set<Action> m_actionChoices;

	uint64_t m_timeStamp;

	void generateMatchSet(std::deque<Classifier<S, Action>> & matchSet, const State<S> & situation)
	{
		matchSet.clear();

		auto unselectedActions = m_actionChoices;

		for (auto && cl : m_population)
		{
			if (cl.condition.contains(situation))
			{
				matchSet.push_back(cl);
				unselectedActions.erase(cl.action);
			}
		}

		// Generate classifiers covering the unselected actions
		for (Action action : unselectedActions)
		{
			auto coveringClassifier = generateCoveringClassifier(situation, action);
			m_population.push_back(coveringClassifier);
			deleteFromPopulation();
			matchSet.push_back(coveringClassifier);
		}
	}

	auto generateCoveringClassifier(const State<S> & situation, Action action)
	{
		// Generate a more general condition than the situation
		auto condition = situation;
		condition.randomGeneralize(m_constants.generalizeProbability);

		// Generate a classifier
		Classifier<S, Action> cl(condition, action, m_timeStamp);
		return cl;
	}

	void deleteFromPopulation()
	{
		// No need to delete classifiers if the sum of numerosity has not met its maximum limit
		uint64_t numerositySum = 0;
		for (auto && c : m_population)
		{
			numerositySum += c.numerosity();
		}
		if (numerositySum < m_constants.maxPopulationClassifierCount)
		{
			return;
		}

		// Prepare a roulette wheel by the weights
		double voteSum;
		std::vector<double> rouletteWheel(std::count(m_population));
		for (auto && c : m_population)
		{
			voteSum += deletionVote(c);
			rouletteWheel.push_back(voteSum);
		}

		// Spin the roulette wheel
		auto rouletteIterator = std::lower_bound(std::begin(rouletteWheel), std::end(rouletteWheel), Random::nextDouble(0, voteSum));
		size_t populationIterator = std::begin(m_population) + std::distance(std::begin(rouletteWheel), rouletteIterator);
		auto && selectedClassifier = *populationIterator;

		// Distrust the selected classifier
		if (selectedClassifier.numerosity() > 1)
			selectedClassifier.decreaseNumerosity();
		else
			m_population.erase(populationIterator);
	}

	double deletionVote(const Classifier<S, Action> & cl)
	{
		double vote = cl.actionSetSize() * cl.numerosity();

		// Calculate the average fitness in the population
		double fitnessSum = 0.0;
		uint64_t numerositySum = 0;
		for (auto && c : m_population)
		{
			fitnessSum += c.fitness();
			numerositySum += c.numerosity();
		}
		double averageFitness = fitnessSum / numerositySum;

		// Consider the fitness
		if ((cl.experience() > m_constants.thetaDel) && (cl.fitness() / cl.numerosity() < averageFitness))
		{
			vote *= averageFitness / (cl.fitness() / cl.numerosity());
		}

		return vote;
	}

public:
	explicit XCS(const XCSConstants & constants, const std::set<Action> & actionChoices) : m_constants(constants), m_actionChoices(actionChoices), m_timeStamp(0) {}
};