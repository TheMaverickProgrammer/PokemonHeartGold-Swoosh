#include "pch.h"
#include "ActionList.h"



void ActionList::insert(std::size_t pos, ClearPreviousActions* clearAction) {
  clearAction->list = this;
  items.insert(items.begin() + pos, (ActionItem*)clearAction);
  clearAction->index = pos;

  updateIndexesFrom(pos);
}

void ActionList::insert(std::size_t pos, ClearAllActions* clearAction) {
  clearAction->list = this;
  items.insert(items.begin() + pos, (ActionItem*)clearAction);
  clearAction->index = pos;

  updateIndexesFrom(pos);
}

void ActionList::add(ClearPreviousActions* clearAction) {
  clearAction->list = this;
  items.push_back((ActionItem*)clearAction);
  clearAction->index = items.size() - 1;
}

void ActionList::add(ClearAllActions* clearAction) {
  clearAction->list = this;
  items.push_back((ActionItem*)clearAction);
  clearAction->index = items.size() - 1;
}