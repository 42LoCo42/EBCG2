#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include "SaveState.hpp"
#include "BoardControl.hpp"

using namespace std;
using namespace rapidjson;

SaveState::SaveState(int x, int y) {
	setupBoard(board, x, y);
	score = 0;
	currentNum = nextNum(6);
}

void SaveState::toString(string& result) {
	Document d;
	d.Parse(TEMPLATE.c_str());

	Value& scoreVal = d["score"];
	scoreVal.SetInt(score);
	Value& maxScoreVal = d["maxScore"];
	scoreVal.SetInt(maxScore);
	Value& numberVal = d["number"];
	scoreVal.SetInt(currentNum);

	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	d.Accept(writer);

	result = buffer.GetString();
}
