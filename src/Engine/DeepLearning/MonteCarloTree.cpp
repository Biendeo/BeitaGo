#include "MonteCarloTree.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <random>

namespace BeitaGo {

	MonteCarloTree::MonteCarloTree(const Board& board) : _board(board), _validMoves(board.GetValidMoves(board.GetWhoseTurn())), _children(board.GetDimensions().X() * board.GetDimensions().Y() + 1), _totalWins(0), _totalSimulations(0) {
		// When initialised, all children will be marked as invalid. Go through all the valid moves,
		// and mark those as valid.
		for (const Grid2& g : _validMoves) {
			int index = Grid2ToIndex(g);
			Board b = _board;
			b.PlacePiece(g, b.GetWhoseTurn());
			b.NextTurn();
			_children[index].SetBoard(b, g);
			_children[index].SetValid(true);
		}
	}

	void MonteCarloTree::RunSimulation() {
		//TODO: Use the heuristic here.
		constexpr double c = 1.500;
		std::default_random_engine randomEngine;
		randomEngine.seed(static_cast<unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));

		_lock.lock();
		std::vector<std::pair<double, Grid2>> movesOrdered;
		for (const Grid2& g : _validMoves) {
			const MonteCarloNode& node = _children[Grid2ToIndex(g)];
			movesOrdered.emplace_back(std::make_pair(static_cast<double>(node.TotalWins()) / node.TotalSimulations() + c * std::sqrt(std::log(_totalSimulations) / node.TotalSimulations()), g));
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

	void MonteCarloTree::InitializeNodes(int n, int maxThreads) {
		// There's n * ((x * y) + 1) simulations.
		const int x = _board.GetDimensions().X();
		const int y = _board.GetDimensions().Y();
		
		std::vector<std::thread> threads;
		for (int i = 0; i < maxThreads; ++i) {
			std::vector<int> indiciesToCheck;
			for (int a = i; a < (n * (x * y + 1)); a += maxThreads) {
				indiciesToCheck.push_back(a % (x * y + 1));
			}

			//TODO: This will cut off some desired executions.
			threads.emplace_back(std::thread([&, indiciesToCheck]() {
				for (const int& a : indiciesToCheck) {
					MonteCarloNode& node = _children[a];
					if (node.IsValid()) {
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
				}
			}));
		}
		for (int i = 0; i < maxThreads; ++i) {
			threads[i].join();
		}
	}

	void MonteCarloTree::RunSimulations(int n, int maxThreads) {
		if (maxThreads == 1) {
			for (int x = 0; x < n; ++x) {
				RunSimulation();
			}
		} else {
			std::vector<std::thread> threads;
			for (int i = 0; i < maxThreads; ++i) {
				//TODO: This will cut off some desired executions.
				threads.emplace_back(std::thread([&]() { this->RunSimulations(n / maxThreads, 1); }));
			}
			for (int i = 0; i < maxThreads; ++i) {
				threads[i].join();
			}
		}
	}

	void MonteCarloTree::RunSimulations(const std::chrono::high_resolution_clock::time_point& endTime, int maxThreads) {
		if (maxThreads == 1) {
			while (std::chrono::high_resolution_clock::now() < endTime) {
				RunSimulation();
			}
		} else {
			std::vector<std::thread> threads;
			for (int i = 0; i < maxThreads; ++i) {
				//TODO: This will cut off some desired executions.
				threads.emplace_back(std::thread([&]() { this->RunSimulations(endTime, 1); }));
			}
			for (int i = 0; i < maxThreads; ++i) {
				threads[i].join();
			}
		}
	}

	Grid2 MonteCarloTree::GetMostLikelyMove() const {
		auto bestMove = std::make_pair(0.0, PASS);
		for (const MonteCarloNode& node : _children) {
			if (node.IsValid() && node.TotalSimulations() > 0) {
				//TODO: Make this heuristic better.
				double score = node.TotalWins() / static_cast<double>(node.TotalSimulations());
				//std::cout << "(" << node.LastMove().X() << ", " << node.LastMove().Y() << ") = " << score << "\n";
				if (score > bestMove.first) {
					bestMove.first = score;
					bestMove.second = node.LastMove();
				}
			}
		}
		return bestMove.second;
	}

	int MonteCarloTree::PassIndex() const {
		return static_cast<int>(_children.size() - 1);
	}

	int MonteCarloTree::Grid2ToIndex(const Grid2& g) const {
		if (g == PASS) {
			return PassIndex();
		} else {
			return g.Y() * _board.GetDimensions().X() + g.X();
		}
	}

	Grid2 MonteCarloTree::IndexToGrid2(int index) const {
		if (index == PassIndex()) {
			return PASS;
		} else {
			return Grid2(index % _board.GetDimensions().X(), index / _board.GetDimensions().X());
		}
	}
}