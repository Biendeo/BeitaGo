#include "DeepLearningAIPlayer.h"

#include <fstream>

#include "Engine.h"
#include "DeepLearning/MonteCarloTree.h"
#include "DeepLearning/NNMonteCarloTree.h"

namespace BeitaGo {
	DeepLearningAIPlayer::DeepLearningAIPlayer(Engine& engine, Color color) : DeepLearningAIPlayer(engine, color, DEFAULT_THINKING_TIME) {}

	DeepLearningAIPlayer::DeepLearningAIPlayer(Engine& engine, Color color, const std::chrono::duration<double>& thinkingTime) : MonteCarloAIPlayer(engine, color, thinkingTime), _heuristicValues{0.0} {}

	DeepLearningAIPlayer::DeepLearningAIPlayer(Engine& engine, Color color, const std::chrono::duration<double>& thinkingTime, const std::string& networkFilePath) : MonteCarloAIPlayer(engine, color, thinkingTime), _heuristicValues{0.0} {
		std::ifstream fin(networkFilePath, std::ios::binary);
		if (fin) {
			dlib::deserialize(_network, fin);
		}
	}

	DeepLearningAIPlayer::DeepLearningAIPlayer(Engine& engine, Color color, const std::chrono::duration<double>& thinkingTime, DeepLearningAIPlayer::NetworkType& network) : MonteCarloAIPlayer(engine, color, thinkingTime), _network(network), _heuristicValues{0.0} {}

	DeepLearningAIPlayer::~DeepLearningAIPlayer() {}

	Grid2 DeepLearningAIPlayer::MakeDecision() const {
		NNMonteCarloTree tree(GetEngine().GetBoard(), _network);
		std::chrono::high_resolution_clock::time_point endTime = std::chrono::high_resolution_clock::now() + std::chrono::duration_cast<std::chrono::seconds>(_thinkingTime);
		//tree.InitializeNodes(1);
		tree.RunSimulations(endTime);
		_heuristicValues = tree.GetAllHeuristicValuesNormalised();
		return tree.GetMostLikelyMove();
	}
	
	std::array<bool, DeepLearningAIPlayer::INPUT_VECTOR_SIZE> DeepLearningAIPlayer::BoardToInputVector() const {
		return BoardToInputVector(GetEngine().GetBoard());
	}
	
	std::array<bool, DeepLearningAIPlayer::INPUT_VECTOR_SIZE> DeepLearningAIPlayer::BoardToInputVector(const Board& board) {
		std::array<bool, INPUT_VECTOR_SIZE> inputVector;
		int index = 0;
		for (int i = 0; i < 2 * HISTORY_USED; ++i) {
			for (int y = 0; y < board.GetDimensions().Y(); ++y) {
				for (int x = 0; x < board.GetDimensions().X(); ++x) {
					if (i % 2 == 0) {
						if (i == 0) {
							inputVector[index] = board.GetTile(Grid2(x, y)) == Color::Black;
						} else {
							std::vector<std::vector<Color>> historyBoard = board.GetPreviousLayout(i / 2);
							inputVector[index] = historyBoard[x][y] == Color::Black;
						}
					} else {
						if (i == 0) {
							inputVector[index] = board.GetTile(Grid2(x, y)) == Color::White;
						} else {
							std::vector<std::vector<Color>> historyBoard = board.GetPreviousLayout(i / 2);
							inputVector[index] = historyBoard[x][y] == Color::White;
						}
					}
					++index;
				}
			}
		}
		for (int y = 0; y < board.GetDimensions().Y(); ++y) {
			for (int x = 0; x < board.GetDimensions().X(); ++x) {
				inputVector[index] = board.GetWhoseTurn() == Color::Black;
				++index;
			}
		}
		return inputVector;
	}

	dlib::matrix<unsigned char, DeepLearningAIPlayer::INPUT_VECTOR_SIZE, 1> DeepLearningAIPlayer::BoardToDlibMatrix() const {
		return BoardToDlibMatrix(GetEngine().GetBoard());
	}

	dlib::matrix<unsigned char, DeepLearningAIPlayer::INPUT_VECTOR_SIZE, 1> DeepLearningAIPlayer::BoardToDlibMatrix(const Board& board) {
		dlib::matrix<unsigned char, INPUT_VECTOR_SIZE, 1> m;
		std::array<bool, INPUT_VECTOR_SIZE> inputVector = BoardToInputVector(board);
		for (int r = 0; r < inputVector.size(); ++r) {
			m(r, 0) = inputVector[r] ? 1 : 0;
		}
		return m;
	}

	std::array<double, DeepLearningAIPlayer::OUTPUT_VECTOR_SIZE> DeepLearningAIPlayer::GetAllHeuristicValues() const {
		return _heuristicValues;
	}
}