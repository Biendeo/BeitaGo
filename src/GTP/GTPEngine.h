#pragma once

#include <algorithm>
#include <string>
#include <vector>

#include "Constants.h"
#include "Engine.h"

class GTPEngine {
	public:

	GTPEngine();
	GTPEngine(double thinkingTime);
	~GTPEngine();

	void Start();

	private:
	enum class CommandType {
		Unknown,
		ProtocolVersion,
		Name,
		Version,
		KnownCommand,
		ListCommands,
		Quit,
		BoardSize,
		ClearBoard,
		Komi,
		FixedHandicap,
		PlaceFreeHandicap,
		SetFreeHandicap,
		Play,
		GenMove,
		Undo,
		TimeSettings,
		TimeLeft,
		FinalScore,
		FinalStatusList,
		LoadSGF,
		RegGenMove,
		ShowBoard
	};

	struct Command {
		public:

	};

	struct GTPVertex {
		public:
		GTPVertex(const std::string& str){
			std::string possiblePass = str.substr(0, 4);
			std::transform(possiblePass.begin(), possiblePass.end(), possiblePass.begin(), ::tolower);
			if (possiblePass == "pass") {
				val = BeitaGo::PASS;
			} else {
				val = BeitaGo::Grid2(LetterToColumn(str[0]), std::stoi(str.substr(1)) - 1);
			}
		}
		GTPVertex(const BeitaGo::Grid2& grid) : val(grid) {}

		operator BeitaGo::Grid2() { return val; }
		operator std::string() {
			if (val != BeitaGo::PASS) {
				return ColumnToLetter(val.X()) + std::to_string(val.Y() + 1);
			} else {
				return "PASS";
			}
		}
		BeitaGo::Grid2& operator=(BeitaGo::Grid2& o) { return val = o; }

		BeitaGo::Grid2 val;
	};

	struct GTPColor {
		public:
		GTPColor(const std::string& color) {
			std::string lower = color;
			std::transform(lower.begin(), lower.end(), lower.begin(), tolower);
			(lower == "white" || lower == "w") ? val = BeitaGo::Color::White : val = BeitaGo::Color::Black;
		}
		GTPColor(BeitaGo::Color color) : val(color) {}

		operator BeitaGo::Color() { return val; }
		operator std::string() { return val == BeitaGo::Color::White ? "white" : "black"; }
		BeitaGo::Color& operator=(BeitaGo::Color color) { return val = color; }

		BeitaGo::Color val;
	};

	struct GTPMove {
		public:
		GTPMove(const std::string& move) : color(BeitaGo::Color::White), vertex(BeitaGo::Grid2(0, 0)) {
			// Find the space:
			int spacePos = 0;
			while (spacePos < move.size() && move[spacePos] != ' ') {
				++spacePos;
			}
			color = GTPColor(move.substr(0, spacePos));
			vertex = GTPVertex(move.substr(spacePos + 1));
		}
		GTPMove(const GTPColor& col, const GTPVertex& vert) : color(col), vertex(vert) {}

		operator std::string() { return static_cast<std::string>(color) + " " + static_cast<std::string>(vertex); }
		GTPColor Color() const { return color; }
		GTPVertex Vertex() const { return vertex; }

		private:
		GTPColor color;
		GTPVertex vertex;
	};

	struct GTPBoolean {
		public:
		GTPBoolean(bool b) : val(b) {}

		operator std::string() { return val ? "true" : "false"; }
		operator bool() { return val; }
		bool& operator=(bool b) { return val = b; }

		bool val;
	};

	static constexpr const int32_t PROTOCOL_VERSION = 2;
	static constexpr const char* NAME = "BeitaGo Dumb AI";
	static constexpr const char* VERSION = "0.0.1";

	static constexpr const int MAX_SIZE = 25;

	bool _exiting;
	BeitaGo::Engine _engine;
	double _thinkingTime;

	void PrintSplashMessage() const;
	void MainLoop();

	void HandleInput(const std::string& input);
	std::string PreprocessInput(const std::string& input) const;

	void PrintProtocolVersion(int id, const std::vector<std::string>& arguments) const;
	void PrintName(int id, const std::vector<std::string>& arguments) const;
	void PrintVersion(int id, const std::vector<std::string>& arguments) const;
	void PrintKnownCommand(int id, const std::vector<std::string>& arguments) const;
	void PrintListCommands(int id, const std::vector<std::string>& arguments) const;

	void BoardSize(int id, const std::vector<std::string>& arguments);
	void ClearBoard(int id, const std::vector<std::string>& arguments);
	void Komi(int id, const std::vector<std::string>& arguments);
	void Play(int id, const std::vector<std::string>& arguments);
	void GenMove(int id, const std::vector<std::string>& arguments);
	void Undo(int id, const std::vector<std::string>& arguments);
	void FinalScore(int id, const std::vector<std::string>& arguments);
	void ShowBoard(int id, const std::vector<std::string>& arguments) const;

	void PrintSuccessResponse(int id, const std::string& message) const;
	void PrintFailureResponse(int id, const std::string& message) const;

	int32_t ProtocolVersion() const;
	std::string Name() const;
	std::string Version() const;

	void PrintBoard() const;
	void RestartBoard(const BeitaGo::Grid2& dimensions);

	static std::vector<std::string> Tokenize(const std::string& str);
	static CommandType StrToCommandType(const std::string& str);
	static char ColumnToLetter(int col);
	static int LetterToColumn(char letter);
};


