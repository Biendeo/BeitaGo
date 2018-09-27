#include "GTPEngine.h"

#include <array>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "DumbAIPlayer.h"
#include "MonteCarloAIPlayer.h"

GTPEngine::GTPEngine() : GTPEngine(BeitaGo::DEFAULT_THINKING_TIME.count()) {}

GTPEngine::GTPEngine(double thinkingTime) : _thinkingTime(thinkingTime) {
	RestartBoard(BeitaGo::Grid2(19, 19));
}

GTPEngine::~GTPEngine() {

}

void GTPEngine::Start() {
	//PrintSplashMessage();
	_exiting = false;
	MainLoop();
}

void GTPEngine::PrintSplashMessage() const {
	std::cout << Name() << "\n" << Version() << "\n";
}

void GTPEngine::MainLoop() {
	while (!_exiting) {
		//PrintBoard();
		//std::cout << "> ";
		std::string input;
		std::getline(std::cin, input);
		HandleInput(input);
	}
}

void GTPEngine::HandleInput(const std::string& input) {
	// Clean the input and tokenize it.
	std::string cleanedInput = PreprocessInput(input);
	std::vector<std::string> tokenizedInput = Tokenize(cleanedInput);

	// If the command starts with an int, that'll be an ID.
	if (tokenizedInput.size() >= 1) {
		bool usingId = false;
		int id;
		try {
			id = std::stoi(tokenizedInput[0]);
			if (id >= 0) {
				usingId = true;
			} else {
				id = -1;
			}
		} catch (std::exception& e) {
			// I guess it didn't work.
			(void)e;
			id = -1;
		}

		if (tokenizedInput.size() >= (usingId ? 2 : 1)) {
			// Then, determine the command used.
			CommandType type = StrToCommandType(tokenizedInput[(usingId ? 1 : 0)]);
			std::vector<std::string> arguments = std::vector<std::string>(tokenizedInput.begin() + (usingId ? 2 : 1), tokenizedInput.end());
			switch (type) {
				case CommandType::ProtocolVersion:
					PrintProtocolVersion(id, arguments);
					break;
				case CommandType::Name:
					PrintName(id, arguments);
					break;
				case CommandType::Version:
					PrintVersion(id, arguments);
					break;
				case CommandType::KnownCommand:
					PrintKnownCommand(id, arguments);
					break;
				case CommandType::ListCommands:
					PrintListCommands(id, arguments);
					break;
				case CommandType::Quit:
					_exiting = true;
					break;
				case CommandType::BoardSize:
					BoardSize(id, arguments);
					break;
				case CommandType::ClearBoard:
					ClearBoard(id, arguments);
					break;
				case CommandType::Komi:
					Komi(id, arguments);
					break;
				case CommandType::Play:
					Play(id, arguments);
					break;
				case CommandType::GenMove:
					GenMove(id, arguments);
					break;
				case CommandType::Undo:
					Undo(id, arguments);
					break;
				case CommandType::FinalScore:
					FinalScore(id, arguments);
					break;
				case CommandType::ShowBoard:
					ShowBoard(id, arguments);
					break;
				default:
					PrintFailureResponse(id, "Unknown command supplied");
			}
		} else {
			PrintFailureResponse(id, "Unknown command supplied");
		}
	} else {
		PrintFailureResponse(-1, "Unknown command supplied");
	}
}

void GTPEngine::PrintProtocolVersion(int id, const std::vector<std::string>& arguments) const {
	PrintSuccessResponse(id, std::to_string(ProtocolVersion()));
}

void GTPEngine::PrintName(int id, const std::vector<std::string>& arguments) const {
	PrintSuccessResponse(id, Name());
}

void GTPEngine::PrintVersion(int id, const std::vector<std::string>& arguments) const {
	PrintSuccessResponse(id, Version());
}

void GTPEngine::PrintKnownCommand(int id, const std::vector<std::string>& arguments) const {
	// I dunno if this should ever error. Just print a success false otherwise.
	if (arguments.size() >= 1) {
		CommandType type = StrToCommandType(arguments[0]);
		if (type != CommandType::Unknown) {
			PrintSuccessResponse(id, GTPBoolean(true));
		} else {
			PrintSuccessResponse(id, GTPBoolean(false));
		}
	} else {
		PrintFailureResponse(id, GTPBoolean(false));
	}
}

void GTPEngine::PrintListCommands(int id, const std::vector<std::string>& arguments) const {
	std::stringstream ss;
	ss << "protocol_version" << "\n";
	ss << "name" << "\n";
	ss << "version" << "\n";
	ss << "known_command" << "\n";
	ss << "list_commands" << "\n";
	ss << "quit" << "\n";
	ss << "boardsize" << "\n";
	ss << "clear_board" << "\n";
	ss << "komi" << "\n";
	ss << "play" << "\n";
	ss << "genmove" << "\n";
	ss << "undo" << "\n";
	ss << "score" << "\n";
	ss << "showboard";
	PrintSuccessResponse(id, ss.str());
}

void GTPEngine::BoardSize(int id, const std::vector<std::string>& arguments) {
	if (arguments.size() >= 1) {
		try {
			int size = std::stoi(arguments[0]);
			if (size > MAX_SIZE) {
				PrintFailureResponse(id, "unacceptable size");
			} else {
				RestartBoard(BeitaGo::Grid2(size, size));
				PrintSuccessResponse(id, "");
			}
		} catch (std::exception& e) {
			// I guess it didn't work.
			(void)e;
			PrintFailureResponse(id, "syntax error");
		}
	} else {
		PrintFailureResponse(id, "syntax error");
	}
}

void GTPEngine::ClearBoard(int id, const std::vector<std::string>& arguments) {
	RestartBoard(_engine.GetBoard().GetDimensions());
	PrintSuccessResponse(id, "");
}

void GTPEngine::Komi(int id, const std::vector<std::string>& arguments) {
	if (arguments.size() >= 1) {
		try {
			double komi = std::stod(arguments[0]);
			_engine.GetBoard().SetKomi(komi);
			PrintSuccessResponse(id, "");
		} catch (std::exception& e) {
			(void)e;
			PrintFailureResponse(id, "syntax error");
		}
	} else {
		PrintFailureResponse(id, "syntax error");
	}
}

void GTPEngine::Play(int id, const std::vector<std::string>& arguments) {
	if (arguments.size() == 1) {
		try {
			GTPMove move(arguments[0]);
			if (_engine.GetBoard().IsMoveValid(move.Vertex(), move.Color())) {
				_engine.GetBoard().PlacePiece(move.Vertex(), move.Color());
			}
			PrintSuccessResponse(id, "");
		} catch (std::exception& e) {
			(void)e;
			PrintFailureResponse(id, "syntax error");
		}
	} else if (arguments.size() >= 2) {
		try {
			GTPMove move(arguments[0] + " " + arguments[1]);
			if (_engine.GetBoard().IsMoveValid(move.Vertex(), move.Color())) {
				_engine.GetBoard().PlacePiece(move.Vertex(), move.Color());
			}
			PrintSuccessResponse(id, "");
		} catch (std::exception& e) {
			(void)e;
			PrintFailureResponse(id, "syntax error");
		}
	} else {
		PrintFailureResponse(id, "syntax error");
	}
}

void GTPEngine::GenMove(int id, const std::vector<std::string>& arguments) {
	if (arguments.size() >= 1) {
		try {
			GTPColor color(arguments[0]);
			if (static_cast<BeitaGo::Color>(color) == BeitaGo::Color::Black) {
				GTPVertex move(dynamic_cast<BeitaGo::AIPlayer&>(_engine.GetPlayer1()).MakeDecision());
				_engine.GetPlayer1().ActDecision(move);
				PrintSuccessResponse(id, move);
			} else {
				// Assume it to be white.
				GTPVertex move(dynamic_cast<BeitaGo::AIPlayer&>(_engine.GetPlayer2()).MakeDecision());
				_engine.GetPlayer2().ActDecision(move);
				PrintSuccessResponse(id, move);
			}
		} catch (std::exception& e) {
			PrintFailureResponse(id, e.what());
		}
	} else {
		PrintFailureResponse(id, "syntax error");
	}
}

void GTPEngine::Undo(int id, const std::vector<std::string>& arguments) {
	if (_engine.GetBoard().GetTurnCount() > 1) {
		_engine.GetBoard().RewindBoard(1);
		PrintSuccessResponse(id, "");
	} else {
		PrintFailureResponse(id, "cannot undo");
	}
}

void GTPEngine::FinalScore(int id, const std::vector<std::string>& arguments) {
	double score = _engine.GetBoard().Score();
	std::stringstream ss;
	ss << std::fixed << std::setprecision(1);
	if (score == 0.0) {
		PrintSuccessResponse(id, "0");
	} else if (score > 0.0) {
		ss << "W+" << score;
		PrintSuccessResponse(id, ss.str());
	} else {
		ss << "B+" << -score;
		PrintSuccessResponse(id, ss.str());
	}
}

void GTPEngine::ShowBoard(int id, const std::vector<std::string>& arguments) const {
	std::cout << "=\n";
	std::cout << "00\nBoard state:\n";
	PrintBoard();
	std::cout << "\n";
}

void GTPEngine::PrintSuccessResponse(int id, const std::string& message) const {
	std::cout << "=";
	if (id != -1) {
		std::cout << id;
	}
	std::cout << " " << message << "\n\n";
}

void GTPEngine::PrintFailureResponse(int id, const std::string& message) const {
	std::cout << "?";
	if (id != -1) {
		std::cout << id;
	}
	std::cout << " " << message << "\n\n";
}

int32_t GTPEngine::ProtocolVersion() const {
	return PROTOCOL_VERSION;
}

std::string GTPEngine::Name() const {
	return NAME;
}

std::string GTPEngine::Version() const {
	return VERSION;
}

std::string GTPEngine::PreprocessInput(const std::string& input) const {
	// ALl control characters except for HT and LF should be removed.
	std::string newString;

	bool inComment = false;

	for (char c : input) {
		if (inComment && c == '\n') {
			inComment = false;
			newString += c;
		} else if (!inComment && c < ' ' && c != '\t' && c != '\n') {
			// Skip it.
		} else if (!inComment && c == '\t') {
			newString += ' ';
		} else if (!inComment && c == '#') {
			inComment = true;
		} else if (!inComment) {
			newString += c;
		}
	}

	return newString;
}

void GTPEngine::PrintBoard() const {
	for (int y = _engine.GetBoard().GetDimensions().Y() - 1; y >= 0; --y) {
		std::cout << " ";
		for (int x = 0; x < _engine.GetBoard().GetDimensions().X(); ++x) {
			BeitaGo::Color color = _engine.GetBoard().GetTile(BeitaGo::Grid2(x, y));
			if (color == BeitaGo::Color::Black) {
				std::cout << "X";
			} else if (color == BeitaGo::Color::White) {
				std::cout << "O";
			} else {
				std::cout << ".";
			}
		}
		std::cout << "\n";
	}
}

void GTPEngine::RestartBoard(const BeitaGo::Grid2& dimensions) {
	_engine.NewGame(dimensions, new BeitaGo::MonteCarloAIPlayer(_engine, BeitaGo::Color::Black, std::chrono::duration<double>(_thinkingTime)), new BeitaGo::MonteCarloAIPlayer(_engine, BeitaGo::Color::White, std::chrono::duration<double>(_thinkingTime)));
}

GTPEngine::CommandType GTPEngine::StrToCommandType(const std::string& str) {
	if (str == "protocol_version") {
		return CommandType::ProtocolVersion;
	} else if (str == "name") {
		return CommandType::Name;
	} else if (str == "version") {
		return CommandType::Version;
	} else if (str == "known_command") {
		return CommandType::KnownCommand;
	} else if (str == "list_commands") {
		return CommandType::ListCommands;
	} else if (str == "quit") {
		return CommandType::Quit;
	} else if (str == "boardsize") {
		return CommandType::BoardSize;
	} else if (str == "clear_board") {
		return CommandType::ClearBoard;
	} else if (str == "komi") {
		return CommandType::Komi;
	} else if (str == "fixed_handicap") {
		return CommandType::FixedHandicap;
	} else if (str == "place_free_handicap") {
		return CommandType::PlaceFreeHandicap;
	} else if (str == "set_free_handicap") {
		return CommandType::SetFreeHandicap;
	} else if (str == "play") {
		return CommandType::Play;
	} else if (str == "genmove") {
		return CommandType::GenMove;
	} else if (str == "undo") {
		return CommandType::Undo;
	} else if (str == "time_settings") {
		return CommandType::TimeSettings;
	} else if (str == "time_left") {
		return CommandType::TimeLeft;
	} else if (str == "final_score") {
		return CommandType::FinalScore;
	} else if (str == "final_status_list") {
		return CommandType::FinalStatusList;
	} else if (str == "loadsgf") {
		return CommandType::LoadSGF;
	} else if (str == "reg_genmove") {
		return CommandType::RegGenMove;
	} else if (str == "showboard") {
		return CommandType::ShowBoard;
	} else {
		return CommandType::Unknown;
	}
}

std::vector<std::string> GTPEngine::Tokenize(const std::string& str) {
	std::vector<std::string> r;

	std::string currentStr;
	for (int i = 0; i < str.size(); ++i) {
		char c = str[i];
		if (c == ' ') {
			if (currentStr != "") {
				r.push_back(currentStr);
				currentStr = "";
			}
		} else if (c == '\"') {
			if (currentStr == "") {
				++i;
				while (str[i] != '\"') {
					currentStr += str[i];
					++i;
				}
				if (currentStr != "") {
					r.push_back(currentStr);
					currentStr = "";
				}
			}
		} else {
			currentStr += c;
		}
	}
	if (currentStr != "") {
		r.push_back(currentStr);
	}
	return r;
}

char GTPEngine::ColumnToLetter(int col)  {
	if (col >= 0 && col <= 24) {
		char c = static_cast<char>(col + 'A');
		if (c >= 'I') {
			++c;
		}
		return c;
	} else {
		throw std::invalid_argument("ColumnToLetter() was given an invalid column number");
	}
}

int GTPEngine::LetterToColumn(char letter) {
	char lower = static_cast<char>(tolower(letter));
	if (lower >= 'a' && lower != 'i' && lower <= 'z') {
		int col = lower - 'a' - (lower > 'i' ? 1 : 0);
		return col;
	} else {
		throw std::invalid_argument("LetterToColumn() was given an invalid letter");
	}
}