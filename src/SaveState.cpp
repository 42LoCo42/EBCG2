#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include "SaveState.hpp"
#include "BoardControl.hpp"

using namespace std;
using namespace rapidjson;

void SaveState::init(int x, int y) {
	board = board_t(x);
	setupBoard(board, x, y);
	score = 0;
	maxScore = 0;
	currentNum = nextNum(6);
}

void SaveState::init(const string& data) {
	Document d;
	d.Parse(data.c_str());

	Value& scoreVal = d["score"];
	score = scoreVal.GetInt();
	Value& maxScoreVal = d["maxScore"];
	maxScore = scoreVal.GetInt();
	Value& numberVal = d["currentNum"];
	currentNum = scoreVal.GetInt();

	Value& boardArray = d["board"];
	board = board_t();
	board.resize(boardArray.Size());
	for(SizeType x=0; x<boardArray.Size(); ++x) {
		Value& columnArray = boardArray[x];

		for(SizeType y=0; y<columnArray.Size(); ++y) {
			Value& cellObj = columnArray[y];
			board[x].push_back(cell(cellObj["num"].GetInt(), cellObj["prio"].GetInt()));
		}
	}
}

string SaveState::toString() {
	Document d;
	Document::AllocatorType& alloc = d.GetAllocator();
	d.Parse(TEMPLATE.c_str());

	Value& scoreVal = d["score"];
	scoreVal.SetInt(score);
	Value& maxScoreVal = d["maxScore"];
	maxScoreVal.SetInt(maxScore);
	Value& numberVal = d["currentNum"];
	numberVal.SetInt(currentNum);

	Value boardArray(kArrayType);
	for(int x=0; x<board.size(); ++x) {
		Value columnArray(kArrayType);

		for(int y=0; y<board[0].size(); ++y) {
			Value cellObj(kObjectType);

			cellObj.AddMember("num", board[x][y].num, alloc);
			cellObj.AddMember("prio", board[x][y].prio, alloc);
			columnArray.PushBack(cellObj, alloc);
		}
		boardArray.PushBack(columnArray, alloc);
	}

	d.AddMember("board", boardArray, alloc);
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	d.Accept(writer);

	return buffer.GetString();
}
