#pragma once
#include <string>

#include "queue.hpp"

void TUIThread(queue<std::string>& msgQueue);
void serverThread(queue<std::string>& msgQueue);
