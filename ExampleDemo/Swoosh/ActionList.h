#pragma once
#include <SFML\Graphics.hpp>
#include <functional>

class ActionItem {
  friend class ActionList;

protected:
  bool isBlocking;

private:
  bool isDoneFlag;
  std::size_t index;

public:
  ActionItem() { isBlocking = isDoneFlag = false; index = -1; }
  virtual void update(double elapsed) = 0;
  virtual void draw(sf::RenderTexture& surface) = 0;
  void markDone() { isDoneFlag = true; }
  const bool isDone() const { return isDoneFlag; }
  const std::size_t getIndex() const { return index; }
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
class ConditionalAction;

class ActionList {
  friend class ClearPreviousActions;
  friend class ClearAllActions;
  friend class ConditionalAction;
private:
  std::vector<ActionItem*> items;
  bool clearFlag;
public:
  void updateIndexesFrom(std::size_t pos) {
    for (pos; pos < items.size(); pos++) {
      items[pos]->index++;
    }
  }

  void insert(std::size_t pos, ActionItem* item) {
    item->index = pos;
    items.insert(items.begin() + pos, item);

    updateIndexesFrom(pos);
  }

  void insert(std::size_t pos, ClearPreviousActions* clearAction);
  void insert(std::size_t pos, ClearAllActions*      clearAction);

  void add(ActionItem* item) {
    items.push_back(item);
    item->index = items.size()-1;
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

  void append(ActionList& list) {
    for (int i = 0; i < list.items.size(); i++) {
      items.push_back(list.items[i]);
    }

    list.items.clear();
  }

  void append(ActionList* list) {
    if (list == nullptr)
      throw std::runtime_error("ActionList is nullptr");

    for (int i = 0; i < list->items.size(); i++) {
      items.push_back(list->items[i]);
    }

    list->items.clear();
  }

  void update(double elapsed) {
    for (int i = 0; i < items.size();) {
      if (items[i]->isDone()) {
        delete items[i];
        items.erase(items.begin() + i);
        continue;
      }

      items[i]->index = (std::size_t) i;
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

class ConditionalAction : public BlockingActionItem {
  friend class ActionList;

private:
  ActionList *branchIfTrue, *branchIfFalse, *list;
  std::function<bool()> condition;
public:
  ConditionalAction(std::function<bool()> condition, ActionList *branchIfTrue, ActionList *branchIfFalse) 
    : condition(condition), branchIfTrue(branchIfTrue), branchIfFalse(branchIfFalse), list(nullptr) {

  }

  virtual void update(double elapsed) {
    if (isDone())
      return;

    if (condition()) {
      list->append(branchIfTrue);
      branchIfFalse->clear();
    }
    else {
      list->append(branchIfFalse);
      branchIfTrue->clear();
    }


    markDone();
  }

  virtual void draw(sf::RenderTexture& surface) {
  }
};