#include "NNMonteCarloTree.h"

#include <random>
#include <utility>

namespace BeitaGo {
	NNMonteCarloTree::NNMonteCarloTree(const Board& board, DeepLearningAIPlayer::NetworkType network) : MonteCarloTree(board), _network(network) {}

	void NNMonteCarloTree::RunSimulation() {
		std::default_random_engine randomEngine;
		randomEngine.seed(static_cast<unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));

		_lock.lock();
		std::vector<std::pair<double, Grid2>> movesOrdered;
		for (const Grid2& g : _validMoves) {
			movesOrdered.emplace_back(std::make_pair(GetHeuristicValue(g, false), g));
		}
		std::sort(movesOrdered.begin(), movesOrdered.end(), [](const std::pair<double, Grid2>& a, const std::pair<double, Grid2>& b) -> bool { return a.first > b.first; });

		// Since there may be multiple moves with the same best score, let's just randomly pick one
		// that's the best.
		std::vector<std::pair<double, Grid2>> bestMoves;
		for (const auto& p : movesOrdered) {
			if (p.first < movesOrdered[0].first) {
				break;
			} else {
				bestMoves.push_back(p);
			}
		}
		_lock.unlock();

		std::uniform_int_distribution<int> validMoveDistribution(0, static_cast<int>(bestMoves.size()) - 1);
		int validMoveIndex = validMoveDistribution(randomEngine);
		MonteCarloNode& node = _children[Grid2ToIndex(bestMoves[validMoveIndex].second)];
		if (node.RunSimulation()) {
			_lock.lock();
			++_totalWins;
			_lock.unlock();
		}
		_lock.lock();
		++_totalSimulations;
		//std::cout << _totalWins << " / " << _totalSimulations << "(" << _totalWins / static_cast<double>(_totalSimulations) * 100.0 << "%)\n";
		_lock.unlock();
	}

	void NNMonteCarloTree::InitializeNodes(int n, int maxThreads) {
		MonteCarloTree::InitializeNodes(n, maxThreads);
	}

	void NNMonteCarloTree::RunSimulations(int n, int maxThreads) {
		MonteCarloTree::RunSimulations(n, maxThreads);
	}

	void NNMonteCarloTree::RunSimulations(const std::chrono::high_resolution_clock::time_point& endTime, int maxThreads) {
		MonteCarloTree::RunSimulations(endTime, maxThreads);
	}

	Grid2 NNMonteCarloTree::GetMostLikelyMove() const {
		return MonteCarloTree::GetMostLikelyMove();
	}

	double NNMonteCarloTree::GetHeuristicValue(const Grid2& g, bool checkIfValid) const {
		if (checkIfValid) {
			bool moveFound = false;
			for (const Grid2& o : _validMoves) {
				if (o == g) {
					moveFound = true;
					break;
				}
			}
			if (!moveFound) {
				return 0.0;
			}
		}

		const MonteCarloNode& node = _children[Grid2ToIndex(g)];
		std::vector<dlib::matrix<unsigned char, DeepLearningAIPlayer::INPUT_VECTOR_SIZE, 1>> inputVector(1);
		inputVector[0] = DeepLearningAIPlayer::BoardToDlibMatrix(_board);
		//TODO: Add this part, I can't seem to do it without compiler errors.
		//std::vector<unsigned long> results = _network(inputVector);
		return 0 + DeepLearningAIPlayer::c * (1 / (1.0 + node.TotalSimulations()));
	}
	
	std::array<double, DeepLearningAIPlayer::OUTPUT_VECTOR_SIZE> NNMonteCarloTree::GetAllHeuristicValues() const {
		std::array<double, DeepLearningAIPlayer::OUTPUT_VECTOR_SIZE> arr{0.0};
		for (const Grid2& g : _validMoves) {
			arr[Grid2ToIndex(g)] = GetHeuristicValue(g, false);
		}
		return arr;
	}
	std::array<double, DeepLearningAIPlayer::OUTPUT_VECTOR_SIZE> NNMonteCarloTree::GetAllHeuristicValuesNormalised() const {
		auto arr = GetAllHeuristicValues();
		double total = 0.0;
		for (const double& d : arr) {
			total += d;
		}
		for (double& d : arr) {
			d /= total;
		}
		return arr;
	}
}