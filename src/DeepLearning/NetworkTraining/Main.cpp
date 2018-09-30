#include <chrono>
#include <cstdio>
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

using namespace BeitaGo;

const std::chrono::duration<double> TRAINING_TIME = std::chrono::duration<double>(10.0);
const std::chrono::duration<double> MATCH_TIME = std::chrono::duration<double>(5.0);

const std::string CURRENT_NETWORK_PATH = "BeitaGo_network.dat";
const std::string CURRENT_TRAINING_GAMES_PATH = "BeitaGo_training_games.dat";
const std::string NEW_NETWORK_PATH = "BeitaGo_newnetwork.dat";
const std::string NEW_NETWORK_SYNC_PATH = "BeitaGo_newnetwork_sync";

void SerializeMoves(const std::pair<std::vector<dlib::matrix<unsigned char>>, std::vector<unsigned long>>& moves, const std::string& filePath) {
	std::ofstream outputFile(filePath);

	for (int i = 0; i < moves.first.size(); ++i) {
		for (int r = 0; r < DeepLearningAIPlayer::INPUT_VECTOR_SIZE; ++r) {
			outputFile << moves.first[i](r, 0) << ",";
		}
		outputFile << moves.second[i] << "\n";
	}

	outputFile.close();
}

std::pair<std::vector<dlib::matrix<unsigned char>>, std::vector<unsigned long>> DeserializeMoves(const std::string& filePath) {
	std::vector<dlib::matrix<unsigned char>> boardPositions;
	std::vector<unsigned long> labels;
	std::ifstream inputFile(filePath);
	if (inputFile) {
		std::string currentLine;
		while (std::getline(inputFile, currentLine)) {
			boardPositions.push_back(dlib::matrix<unsigned char, DeepLearningAIPlayer::INPUT_VECTOR_SIZE, 1>());
			for (int i = 0; i < DeepLearningAIPlayer::INPUT_VECTOR_SIZE; ++i) {
				boardPositions[boardPositions.size() - 1](i, 0) = currentLine[2 * i] == '1' ? 1 : 0;
			}
			labels.push_back(std::stoi(currentLine.substr(2 * DeepLearningAIPlayer::INPUT_VECTOR_SIZE)));
		}
	}
	return std::make_pair(boardPositions, labels);
}

std::pair<std::vector<dlib::matrix<unsigned char>>, std::vector<unsigned long>> GenerateMoves(int numberOfMatches, DeepLearningAIPlayer::NetworkType& network) {
	Engine e;
	auto p = DeserializeMoves(CURRENT_TRAINING_GAMES_PATH);
	for (int i = p.first.size(); i < numberOfMatches; ++i) {
		std::cout << "  Game: " << (i + 1) << " / " << numberOfMatches << "\n";
		e.NewGame(Grid2(DeepLearningAIPlayer::EXPECTED_BOARD_SIZE), new DeepLearningAIPlayer(e, Color::Black, TRAINING_TIME, network), new DeepLearningAIPlayer(e, Color::White, TRAINING_TIME, network));
		while (!e.GetBoard().IsGameOver()) {
			std::cout << "    Move: " << e.GetBoard().GetTurnCount() << "\n";
			DeepLearningAIPlayer& player = dynamic_cast<DeepLearningAIPlayer&>(e.GetCurrentPlayer());

			Grid2 chosenMove = player.MakeDecision();
			p.first.push_back(DeepLearningAIPlayer::BoardToDlibMatrix(e.GetBoard()));
			p.second.push_back(chosenMove == PASS ? DeepLearningAIPlayer::OUTPUT_VECTOR_SIZE - 1 : chosenMove.X() * DeepLearningAIPlayer::EXPECTED_BOARD_SIZE + chosenMove.Y());
			player.ActDecision(chosenMove);
		}
		SerializeMoves(p, CURRENT_TRAINING_GAMES_PATH);
	}
	return p;
}

bool PlayValidation(int numberOfMatches, double winPercentage, DeepLearningAIPlayer::NetworkType& oldNetwork, DeepLearningAIPlayer::NetworkType& newNetwork) {
	int oldWins = 0;
	int newWins = 0;
	for (int i = 0; i < numberOfMatches; ++i) {
		std::cout << "  Game: " << (i + 1) << " / " << numberOfMatches << "\n";
		Engine e;
		if (i % 2 == 0) {
			e.NewGame(Grid2(DeepLearningAIPlayer::EXPECTED_BOARD_SIZE), new DeepLearningAIPlayer(e, Color::Black, MATCH_TIME, oldNetwork), new DeepLearningAIPlayer(e, Color::White, MATCH_TIME, newNetwork));
		} else {
			e.NewGame(Grid2(DeepLearningAIPlayer::EXPECTED_BOARD_SIZE), new DeepLearningAIPlayer(e, Color::Black, MATCH_TIME, newNetwork), new DeepLearningAIPlayer(e, Color::White, MATCH_TIME, oldNetwork));
		}
		while (!e.GetBoard().IsGameOver()) {
			std::cout << "    Move: " << e.GetBoard().GetTurnCount() << "\n";
			e.GetCurrentPlayer().ActDecision(dynamic_cast<AIPlayer&>(e.GetCurrentPlayer()).MakeDecision());
		}
		if (i % 2 == 0 && e.GetBoard().Score() > 0.0 || i % 2 == 1 && e.GetBoard().Score() < 0.0) {
			std::cout << "  New network wins a game\n";
			++newWins;
		} else {
			std::cout << "  Old network wins a game\n";
			++oldWins;
		}
	}

	if (static_cast<double>(newWins) / numberOfMatches >= winPercentage) {
		return true;
	} else {
		return false;
	}
}

void Loop() {
	constexpr int NUM_TRAINING_GAMES = 25000;
	constexpr int NUM_TESTING_GAMES = 400;
	constexpr double WIN_PERCENTAGE = 0.55;

	DeepLearningAIPlayer::NetworkType currentNetwork;

	if (std::ifstream(CURRENT_NETWORK_PATH, std::ios::binary)) {
		std::cout << "Loading previously saved network...\n";
		std::ifstream inputFile(CURRENT_NETWORK_PATH, std::ifstream::binary);
		dlib::deserialize(currentNetwork, inputFile);
		inputFile.close();
	}

	std::cout << "Generating games...\n";
	auto trainingData = GenerateMoves(NUM_TRAINING_GAMES, currentNetwork);

	std::cout << "Training network...\n";
	DeepLearningAIPlayer::NetworkType newNetwork;
	dlib::dnn_trainer<DeepLearningAIPlayer::NetworkType> trainer(newNetwork);
	trainer.set_learning_rate(1);
	trainer.set_min_learning_rate(0.00001);
	trainer.set_mini_batch_size(2048);
	trainer.be_verbose();
	trainer.set_synchronization_file(NEW_NETWORK_SYNC_PATH, std::chrono::seconds(20));
	trainer.train(trainingData.first, trainingData.second);

	newNetwork.clean();
	dlib::serialize(NEW_NETWORK_PATH) << newNetwork;

	std::cout << "Testing new network...\n";
	if (PlayValidation(NUM_TESTING_GAMES, WIN_PERCENTAGE, currentNetwork, newNetwork)) {
		std::cout << "New network is better than current network. Overwriting!\n\n";
		currentNetwork = newNetwork;
		dlib::serialize(CURRENT_NETWORK_PATH) << currentNetwork;
	} else {
		std::cout << "New network failed to beat the current network enough. Retrying...\n\n";
	}

	remove(CURRENT_TRAINING_GAMES_PATH.c_str());
	remove(NEW_NETWORK_PATH.c_str());
	remove(NEW_NETWORK_SYNC_PATH.c_str());
}

int main(int argc, char* argv[]) {
	while (true) {
		Loop();
	}

	return 0;

}