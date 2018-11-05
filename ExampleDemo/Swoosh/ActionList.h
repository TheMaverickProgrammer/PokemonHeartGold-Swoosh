#pragma once
#include <SFML\Graphics.hpp>
#include <iostream>

class ActionItem {
  friend class ActionList;

protected:
  bool isBlocking;

private:
  bool isDoneFlag;
public:
  ActionItem() { isBlocking = isDoneFlag = false; }
  virtual void update(double elapsed) = 0;
  virtual void draw(sf::RenderTexture& surface) = 0;
  void markDone() { isDoneFlag = true; }
  const bool isDone() { return isDoneFlag; }
};

class BlockingActionItem : public ActionItem {
public:
  BlockingActionItem() : ActionItem() {
    isBlocking = true;
  }

  virtual void update(double elapsed) = 0;
  virtual void draw(sf::RenderTexture& surface) = 0;
};

class ClearPreviousActions;

class ActionList {
  friend class ClearPreviousActions;
private:
  std::vector<ActionItem*> items;
  std::vector<ActionItem*> drawables;
  bool clearFlag;
public:
  void add(ActionItem* item) {
    items.push_back(item);
  }

  void add(ClearPreviousActions* clearAction);

  const bool isEmpty() {
    return items.size();
  }

  void clear() {
    for (auto item : items) {
      delete item;
    }

    items.clear();
    drawables.clear();
  }

  void update(double elapsed) {
    drawables.clear();

    aftercleanup:
    int index = 0;
    for (auto item : items) {
      if (item->isDone()) {
        delete item;
        items.erase(items.begin() + index);
        continue;
      }

      drawables.push_back(item);
      index++;

      item->update(elapsed);

      if (clearFlag) {
        clearFlag = false;
        drawables.clear();
        goto aftercleanup; // abort and return next to cleanup
      }

      if (item->isBlocking) {
        break;
      }
    }
  }

  void draw(sf::RenderTexture& surface) {
    for (auto item : drawables) {
      item->draw(surface);
    }
  }

  ActionList() { clearFlag = false;  }

  ~ActionList() { 
    clear();
  }
};


class ClearPreviousActions : public BlockingActionItem {
  friend class ActionList;

private:
  ActionList* list;

public:
  ClearPreviousActions() : list(nullptr) {
    
  }

  virtual void update(double elapsed) {
   for (int i = 0; i < list->items.size(); i++) {
      ActionItem* item = list->items[i];
      if (item != this) {
        delete item;
        list->items.erase(list->items.begin() + i);
        continue;
      }
      else {
        break;
      }
    }

   list->clearFlag = true;
    markDone();
  }

  virtual void draw(sf::RenderTexture& surface) {
  }
};

void ActionList::add(ClearPreviousActions* clearAction) {
  clearAction->list = this;
  items.push_back((ActionItem*)clearAction);
}