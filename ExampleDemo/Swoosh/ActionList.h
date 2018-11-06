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
class ClearAllActions;

class ActionList {
  friend class ClearPreviousActions;
  friend class ClearAllActions;
private:
  std::vector<ActionItem*> items;
  bool clearFlag;
public:
  void add(ActionItem* item) {
    items.push_back(item);
  }

  void add(ClearPreviousActions* clearAction);
  void add(ClearAllActions*      clearAction);

  const bool isEmpty() {
    return items.size();
  }

  void clear() {
    for (auto item : items) {
      delete item;
    }

    items.clear();
  }

  void update(double elapsed) {
    for (int i = 0; i < items.size();) {
      if (items[i]->isDone()) {
        delete items[i];
        items.erase(items.begin() + i);
        continue;
      }

      items[i]->update(elapsed);

      if (clearFlag) {
        clearFlag = false;
        i = 0; 
        continue; // startover, the list has been modified
      }

      if (items[i]->isBlocking) {
        break;
      }

      i++;
    }
  }

  void draw(sf::RenderTexture& surface) {
    for (auto item : items) {
      item->draw(surface);
      if (item->isBlocking) {
        break;
      }
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
    if (isDone())
      return;

    for (int i = 0; i < list->items.size();) {
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

class ClearAllActions : public BlockingActionItem {
  friend class ActionList;

private:
  ActionList* list;

public:
  ClearAllActions() : list(nullptr) {

  }

  virtual void update(double elapsed) {
    if (isDone())
      return;

    // Delete and remove everything but this one
    for (int i = 0; i < list->items.size();) {
      ActionItem* item = list->items[i];
      if (item != this) {
        delete item;
        list->items.erase(list->items.begin() + i);
        continue;
      }
      i++;
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

void ActionList::add(ClearAllActions* clearAction) {
  clearAction->list = this;
  items.push_back((ActionItem*)clearAction);
}