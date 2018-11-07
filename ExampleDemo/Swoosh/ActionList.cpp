#include "pch.h"
#include "ActionList.h"


void ActionList::add(ClearPreviousActions* clearAction) {
  clearAction->list = this;
  items.push_back((ActionItem*)clearAction);
}

void ActionList::add(ClearAllActions* clearAction) {
  clearAction->list = this;
  items.push_back((ActionItem*)clearAction);
}