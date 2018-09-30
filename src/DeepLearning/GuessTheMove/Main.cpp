#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <random>
#include <utility>
#include <vector>

#include <dlib/dnn.h>
#include <dlib/data_io.h>

#include "DeepLearningAIPlayer.h"
#include "Engine.h"
#include "Grid2.h"
#include "MonteCarloAIPlayer.h"

// This program is simply to test training a neural network with dlib.
// The aim is to play the game using the Monte Carlo Tree Search, and see if a neural network can
// train itself to predict the moves that are going to be made. The neural network is not playing
// the game itself though.
// This is not a long term training model, especially since it only trains itself to match the
// MCTS search and not to exceed it.

using namespace BeitaGo;

Engine SetupEngine() {
	Engine e;
	MonteCarloAIPlayer* player1 = new MonteCarloAIPlayer(e, Color::Black);
	MonteCarloAIPlayer* player2 = new MonteCarloAIPlayer(e, Color::White);
	e.NewGame(Grid2(DeepLearningAIPlayer::EXPECTED_BOARD_SIZE, DeepLearningAIPlayer::EXPECTED_BOARD_SIZE), player1, player2);
	return e;
}

std::pair<std::vector<dlib::matrix<unsigned char>>, std::vector<unsigned long>> GenerateData(int size, std::string dataPath, std::string labelsPath) {
	std::vector<dlib::matrix<unsigned char>> data;
	std::vector<unsigned long> labels;

	std::default_random_engine randomEngine;
	randomEngine.seed(static_cast<unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
	std::uniform_int_distribution<int> moveNumDistribution(0, 50);

	if (std::experimental::filesystem::exists(dataPath) && std::experimental::filesystem::exists(labelsPath)) {
		// Data is serialised as a csv of 1s and 0s (1 if true, 0 if false). Commas separate the 
		// individual fields and newlines separate the overall vectors.
		// Labels are serialised as a csv of just the ints (which is just each int on their own line).
		std::string inputString;

		std::ifstream dataIn(dataPath);
		while (std::getline(dataIn, inputString)) {
			dlib::matrix<unsigned char, DeepLearningAIPlayer::INPUT_VECTOR_SIZE, 1> mat;
			for (int i = 0; i < inputString.size(); i += 2) {
				mat(i / 2, 0) = inputString[i] == '1' ? 1 : 0;
			}
			data.push_back(mat);
		}
		dataIn.close();

		std::ifstream labelsIn(labelsPath);
		while (std::getline(labelsIn, inputString)) {
			unsigned char label = std::stoi(inputString);
			labels.push_back(label);
		}
		labelsIn.close();

	}

	while (data.size() < size && labels.size() < size) {
		std::cout << "Generating data: " << data.size() + 1 << "/" << size << "\n";
		Engine e;
		e.NewGame(Grid2(DeepLearningAIPlayer::EXPECTED_BOARD_SIZE, DeepLearningAIPlayer::EXPECTED_BOARD_SIZE), new MonteCarloAIPlayer(e, Color::Black), new MonteCarloAIPlayer(e, Color::White));

		// Play a random number of random moves.
		int randomNumberOfMoves = moveNumDistribution(randomEngine);
		while (e.GetBoard().GetTurnCount() < randomNumberOfMoves) {
			std::vector<Grid2> validMoves = e.GetBoard().GetValidMoves(e.GetBoard().GetWhoseTurn());
			std::uniform_int_distribution<int> validMoveDistribution(0, static_cast<int>(validMoves.size() - 1));
			e.GetCurrentPlayer().ActDecision(validMoves[validMoveDistribution(randomEngine)]);
		}
		// Now we've got the board. Let's get the board as an input vector.
		auto inputVector = DeepLearningAIPlayer::BoardToInputVector(e.GetBoard());
		// And let's predict the move that is generated from this move.
		auto decidedMove = dynamic_cast<MonteCarloAIPlayer&>(e.GetCurrentPlayer()).MakeDecision();
		unsigned long decidedMoveValue = decidedMove == PASS ? DeepLearningAIPlayer::OUTPUT_VECTOR_SIZE - 1 : decidedMove.X() * DeepLearningAIPlayer::EXPECTED_BOARD_SIZE + decidedMove.Y();

		// Let's convert the input vector into a dlib matrix.
		dlib::matrix<unsigned char, DeepLearningAIPlayer::INPUT_VECTOR_SIZE, 1> inMatrix;
		for (int r = 0; r < inputVector.size(); ++r) {
			inMatrix(r, 0) = inputVector[r] ? 1 : 0;
		}

		// Now we must serialise the data.
		std::ofstream dataOut(dataPath, std::ofstream::out | std::ofstream::app);
		bool first = true;
		for (const bool& b : inputVector) {
			if (!first) {
				dataOut << ",";
			}
			first = false;
			dataOut << b ? "1" : "0";
		}
		dataOut << "\n";
		dataOut.close();
		data.push_back(inMatrix);

		// And the label.
		std::ofstream labelsOut(labelsPath, std::ofstream::out | std::ofstream::app);
		labelsOut << decidedMoveValue << "\n";
		labelsOut.close();
		labels.push_back(decidedMoveValue);

	}

	return std::make_pair(data, labels);
}

int main(int argc, char* argv[]) {
	constexpr int TRAINING_SIZE = 600;
	const std::string TRAINING_DATA_PATH = "GuessTheMove_training_data.csv";
	const std::string TRAINING_LABELS_PATH = "GuessTheMove_training_labels.csv";
	constexpr int TESTING_SIZE = 100;
	const std::string TESTING_DATA_PATH = "GuessTheMove_testing_data.csv";
	const std::string TESTING_LABELS_PATH = "GuessTheMove_testing_labels.csv";

	auto training = GenerateData(TRAINING_SIZE, TRAINING_DATA_PATH, TRAINING_LABELS_PATH);
	auto testing = GenerateData(TESTING_SIZE, TESTING_DATA_PATH, TESTING_LABELS_PATH);

	using network_type = dlib::loss_multiclass_log<dlib::fc<DeepLearningAIPlayer::OUTPUT_VECTOR_SIZE, dlib::relu<dlib::fc<100, dlib::relu<dlib::input<dlib::matrix<unsigned char>>>>>>>;

	network_type net;
	dlib::dnn_trainer<network_type> trainer(net);
	trainer.set_learning_rate(1);
	trainer.set_min_learning_rate(0.00001);
	trainer.set_mini_batch_size(128);
	trainer.be_verbose();
	trainer.set_synchronization_file("GuessTheMove_sync", std::chrono::seconds(20));
	trainer.train(training.first, training.second);

	net.clean();
	dlib::serialize("GuessTheMove_net.dat") << net;
	
	std::vector<unsigned long> predictedLabels = net(training.second);

	int correct = 0;
	int wrong = 0;
	for (int i = 0; i < predictedLabels.size(); ++i) {
		if (predictedLabels[i] == testing.second[i]) {
			++correct;
		} else {
			++wrong;
		}
	}

	std::cout << "The network guessed " << correct << " of " << (correct + wrong) << " labels!\n";

	return 0;
	
}