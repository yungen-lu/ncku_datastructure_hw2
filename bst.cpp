#include <cctype>
#include <exception>
#include <fstream>
#include <iostream>
#include <limits>
#include <queue>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <string>
#include <vector>
class TreeNode {
 public:
  int value = 0;
  TreeNode *left = nullptr;
  TreeNode *right = nullptr;
  TreeNode() {}
  TreeNode(int x) : value(x) {}
  TreeNode(int x, TreeNode *left, TreeNode *right) : value(x), left(left), right(right) {}
};
class BST {
 public:
  enum printType { prefix, infix, postfix, level };

  BST(){};
  void InsertNumber(int number);
  void DeleteNumber(int targetValue);
  void SearchNumber(int targetValue);
  void PrintALL();
  TreeNode *getRoot() { return root; }

 private:
  TreeNode *root = nullptr;
  bool deleted = false;

  TreeNode *deleteNode(TreeNode *nodeRoot, int targetValue);
  TreeNode *searchNode(TreeNode *nodeRoot, int targetValue);
  TreeNode *insertNode(TreeNode *nodeRoot, int number);
  void printTree(TreeNode *nodeRoot, printType type);
  void levelPrint();
};

class mainMenu {
 public:
  enum mainMenuOptions { first, second, zero, error };

  int loop();
  void display() const;
  char getUserInput() const;
  mainMenuOptions parseUserInput(char input) const;
};
class partOne {
 public:
  enum partOneOptions { I, D, S, P, R, error };

  partOne() {}  // construct our BST in init
  int loop();
  void display() const;
  char getUserInput() const;
  partOneOptions parseUserInput(char input) const;

 private:
  BST partOneBst = BST();
};
class FileIO {
 public:
  std::vector<int> vectorOfInt;
  std::string fileName;
  std::ifstream fileBuffer;
  FileIO() { getFileName(); }  // try to get the filename at initialization

  void openAndConvert();

 private:
  void getFileName();
  void convertFileBufferToVector();
};

class partTwo {
 public:
  partTwo() {}  // construct our BST in init
  void findMeaty();

 private:
  BST partTwoBst = BST();
  int swordLocation;
  int meatyLocation;
  int trapIndex;
  std::stack<TreeNode *> stackOfTreeNodePtr;
  std::vector<int> vectorOfPath;

  void createBst(std::vector<int> &vectorOfInt);
  void getSwordLocation();
  void getMeatyLocation();
  void getTrapIndex();
  std::vector<int> findNodesToDelete();
  void traversBstInPostOrder(TreeNode *nodeRoot, std::string &target, std::vector<int> &targetNodesValues);
  void findPathToSword(TreeNode *nodeRoot, bool diff);
  TreeNode *findNearestAnc(TreeNode *initNode);
  void findPathToMeaty(TreeNode *nodeRoot);
  void displayShortestPath();
};
int main() {
  auto menu = mainMenu();
  menu.loop();
}

// `printInvalidOptionError`, `handleExceptionError`, `handleCinError` and `parseMultipleInput` are utilty functions
// that will be used in multple places

/**
 * print error messeg to stderr
 */
void printInvalidOptionError() { std::cerr << "Invalid input, please enter valid value" << std::endl; }
void handleExceptionError(const std::invalid_argument &e) { std::cerr << e.what() << std::endl; }
/**
 * clear stdin after error and throw a error
 * @throws throw error when this function is called
 */
void handleCinError() {
  std::cin.clear();
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  throw std::invalid_argument("Invalid input, please enter valid value");
}
/**
 * get multiple inputs(int) through stdin and convert them to a vector
 * @return a vector of int
 * @throws when user enterd invalid input
 */
std::vector<int> parseMultipleInput() {
  int number;
  std::vector<int> vectorOfInt;
  while (!std::cin.fail()) {
    std::cin >> number;
    if (number == -1) {  // get values through stdin until user enter -1
      break;
    }
    if (!std::cin.fail()) {
      vectorOfInt.push_back(number);
    } else {  // if user enter a invalid value e.g "a b" or "1.1 2.2" clear the input buffer
      handleCinError();
      continue;
    }
  }
  return vectorOfInt;
}
// BST class

/**
 * insert node with value `number` to BST
 * @param number the value to insert
 * @throws catch error when insert `number` already exists and throw it to outer scope
 */
void BST::InsertNumber(int number) { root = insertNode(root, number); }
/**
 * delete node that contain `targetValue` from BST
 * @param targetValue the value that we want to delete
 * @throws throw error when node with value `targetValue` does not exist
 * @return true when delete is success, false when fail
 */
void BST::DeleteNumber(int targetValue) {
  deleted = false;
  root = deleteNode(root, targetValue);
  if (deleted == false) {
    std::string errorString;
    errorString = "Number " + std::to_string(targetValue) + " is not exists.";
    throw std::invalid_argument(errorString);
  }
}

/**
 * search node that contain `targetValue` from BST
 * @param targetValue the value that we want to search
 * @throws throw error when node with value `targetValue` does not exist
 * @return true when search is success, false when fail
 */
void BST::SearchNumber(int targetValue) {
  bool result = searchNode(root, targetValue);
  if (result == false) {
    std::string errorString;
    errorString = "SORRY. " + std::to_string(targetValue) + " is not found.";
    throw std::invalid_argument(errorString);
  }
}
/**
 * print BST in prefix, infix, post, level, order
 */
void BST::PrintALL() {
  std::cout << "The tree in prefix order :";
  printTree(root, prefix);
  std::cout << std::endl;
  std::cout << "The tree in infix order :";
  printTree(root, infix);
  std::cout << std::endl;
  std::cout << "The tree in post order :";
  printTree(root, postfix);
  std::cout << std::endl;
  std::cout << "The tree in level order :";
  levelPrint();
  std::cout << std::endl;
}
// private methods
/**
 * delete node that contains `targetValue`
 * @param nodeRoot the root of tree in the current function call
 * @param targetValue the value that we want to delete
 * @return  the new root of the current node
 */
TreeNode *BST::deleteNode(TreeNode *nodeRoot, int targetValue) {
  if (nodeRoot == nullptr) {
    return nullptr;
  }
  if (nodeRoot->value == targetValue) {
    deleted = true;
    if ((nodeRoot->left == nullptr) && (nodeRoot->right == nullptr)) {
      delete nodeRoot;
      return nullptr;
    } else if (nodeRoot->left == nullptr) {
      TreeNode *newRoot = nodeRoot->right;
      delete nodeRoot;
      return newRoot;
    } else if (nodeRoot->right == nullptr) {
      TreeNode *newRoot = nodeRoot->left;
      delete nodeRoot;
      return newRoot;
    } else {
      TreeNode *ptr = nodeRoot->right;
      if (ptr->left == nullptr) {
        ptr->left = nodeRoot->left;
        delete nodeRoot;
        return ptr;
      }
      TreeNode *parent;
      do {
        parent = ptr;
        ptr = ptr->left;
      } while (ptr->left);
      if (ptr->right) {
        parent->left = ptr->right;
      } else {
        parent->left = nullptr;
      }
      ptr->left = nodeRoot->left;
      ptr->right = nodeRoot->right;
      delete nodeRoot;
      return ptr;
    }
  }
  if (targetValue > nodeRoot->value) {
    nodeRoot->right = deleteNode(nodeRoot->right, targetValue);
  } else {
    nodeRoot->left = deleteNode(nodeRoot->left, targetValue);
  }
  return nodeRoot;
}
/**
 * search node that contain `targetValue` from BST
 * @param nodeRoot the root of tree in the current function call
 * @param targetValue the value that we want to search
 * @return pointer to the node if it  exist else nullptr
 */
TreeNode *BST::searchNode(TreeNode *nodeRoot, int targetValue) {
  if (nodeRoot == nullptr) {
    return nullptr;
  }
  if (targetValue == nodeRoot->value) {
    return nodeRoot;
  } else if (targetValue > nodeRoot->value) {
    return searchNode(nodeRoot->right, targetValue);
  } else {
    return searchNode(nodeRoot->left, targetValue);
  }
}
/**
 * insert value `number` to BST
 * @param nodeRoot the root of tree in the current function call
 * @param number the value we want to insert
 * @return  the new root of the current node
 */
TreeNode *BST::insertNode(TreeNode *nodeRoot, int number) {
  if (nodeRoot == nullptr) {
    auto *newNode = new TreeNode(number);
    return newNode;
  }
  if (number == nodeRoot->value) {
    std::string errorString;
    errorString = "Error: Number " + std::to_string(number) + " exists.";
    throw std::invalid_argument(errorString);
  }
  if (number > nodeRoot->value) {
    nodeRoot->right = insertNode(nodeRoot->right, number);
  } else {
    nodeRoot->left = insertNode(nodeRoot->left, number);
  }
  return nodeRoot;
}
/**
 * print BST according to the printType
 * @param nodeRoot the root of tree in the current function call
 * @param type the `printType` we want to print
 */
void BST::printTree(TreeNode *nodeRoot, printType type) {
  if (nodeRoot == nullptr) {
    return;
  }
  if (type == prefix) {
    std::cout << ' ' << nodeRoot->value;
  }
  printTree(nodeRoot->left, type);
  if (type == infix) {
    std::cout << ' ' << nodeRoot->value;
  }
  printTree(nodeRoot->right, type);
  if (type == postfix) {
    std::cout << ' ' << nodeRoot->value;
  }
}
/**
 * print BST in level order
 */
void BST::levelPrint() {
  if (root == nullptr) {
    return;
  }
  std::queue<TreeNode *> queueOfTreeNodes;
  queueOfTreeNodes.push(root);
  while (!queueOfTreeNodes.empty()) {
    TreeNode *frontNode = queueOfTreeNodes.front();
    queueOfTreeNodes.pop();
    std::cout << ' ' << frontNode->value;
    if (frontNode->left) {
      queueOfTreeNodes.push(frontNode->left);
    }
    if (frontNode->right) {
      queueOfTreeNodes.push(frontNode->right);
    }
  }
}
// mainMenu class

/**
 * a loop that loop through output -> userinput -> parseuserinput -> excute functions
 * @return return 0 when user exit the program
 */
int mainMenu::loop() {
  while (true) {
    char input;
    display();
    try {
      input = getUserInput();
    } catch (const std::invalid_argument &e) {
      handleExceptionError(e);
      continue;
    }
    switch (parseUserInput(input)) {
      case first: {
        auto partone = partOne();
        partone.loop();
        break;
      }
      case second: {
        auto parttwo = partTwo();
        parttwo.findMeaty();
        break;
      }
      case zero:
        return 0;

      case error: {
        printInvalidOptionError();
        break;
        default:
          printInvalidOptionError();
      }
    }
  }
}
/**
 * output the main menu context to stdout
 */
void mainMenu::display() const {
  std::cout << "(1)Binary searching Tree." << std::endl;
  std::cout << "(2)Finding Meaty." << std::endl;
  std::cout << "(0)Escape and face to music next year." << std::endl;
}
/**
 * get user input and put the value to input(char)
 * @throw throw error when user enter invalid input
 * @return return input
 */
char mainMenu::getUserInput() const {
  char input;
  std::cin >> input;
  if (std::cin.fail()) {
    handleCinError();
  }
  return input;
}
/**
 * parse user input
 *@param parse target
 * @return enum value according to parse result
 */
mainMenu::mainMenuOptions mainMenu::parseUserInput(char input) const {
  switch (input) {
    case '1':
      return first;
    case '2':
      return second;
    case '0':
      return zero;
    default:
      return error;
  }
}
// partOne class

/**
 * a loop that loop through output -> userinput -> parseuserinput -> excute functions
 * @return return 0 when user exit the program
 */
int partOne::loop() {
  while (true) {
    display();
    char input;
    try {
      input = getUserInput();
    } catch (const std::invalid_argument &e) {
      handleExceptionError(e);
      return 0;
    }
    switch (parseUserInput(input)) {
      case I: {
        std::cout << "Enter numbers: ";
        std::vector<int> vectorOfInt;
        try {
          vectorOfInt = parseMultipleInput();
        } catch (std::invalid_argument &e) {
          handleExceptionError(e);
          continue;
        }
        for (int i : vectorOfInt) {
          try {
            partOneBst.InsertNumber(i);
          } catch (std::invalid_argument &e) {
            handleExceptionError(e);
            continue;
          }
          std::cout << "Number " << i << " is inserted." << std::endl;
        }
        break;
      }
      case D: {
        std::cout << "Enter numbers to deleted : ";
        std::vector<int> vectorOfInt;
        try {
          vectorOfInt = parseMultipleInput();
        } catch (std::invalid_argument &e) {
          handleExceptionError(e);
          continue;
        }
        for (int i : vectorOfInt) {
          try {
            partOneBst.DeleteNumber(i);
          } catch (std::invalid_argument &e) {
            handleExceptionError(e);
            continue;
          }
          std::cout << "Number " << i << " is deleted." << std::endl;
        }
        break;
      }
      case S: {
        std::cout << "Enter elements to searching : ";
        std::vector<int> vectorOfInt;
        try {
          vectorOfInt = parseMultipleInput();
        } catch (std::invalid_argument &e) {
          handleExceptionError(e);
          continue;
        }
        for (int i : vectorOfInt) {
          try {
            partOneBst.SearchNumber(i);
          } catch (std::invalid_argument &e) {
            handleExceptionError(e);
            continue;
          }
          std::cout << "Bingo! " << i << " is found." << std::endl;
        }
        break;
      }
      case P: {
        partOneBst.PrintALL();
        break;
      }
      case error: {
        printInvalidOptionError();
        break;
      }
      case R:
        return 0;

      default:
        printInvalidOptionError();
    }
  }
}
/**
 * output the partone menu context to stdout
 */
void partOne::display() const {
  std::cout << "(I)nsert a number." << std::endl;
  std::cout << "(D)elete a number." << std::endl;
  std::cout << "(S)earch a number." << std::endl;
  std::cout << "(P)rint 4 kinds of order." << std::endl;
  std::cout << "(R)eturn" << std::endl;
}
/**
 * get user input and put the value to input(char)
 * @throw throw error when user enter invalid input
 * @return return input
 */
char partOne::getUserInput() const {
  char input;
  std::cin >> input;
  if (std::cin.fail()) {
    handleCinError();
  }
  return input;
}
/**
 * parse user input
 * @param parse target
 * @return enum value according to parse result
 */
partOne::partOneOptions partOne::parseUserInput(char input) const {
  switch (static_cast<char>(std::toupper(static_cast<unsigned char>(input)))) {
    case 'I':
      return I;
    case 'D':
      return D;
    case 'S':
      return S;
    case 'P':
      return P;
    case 'R':
      return R;
    default:
      return error;
  }
}
// FileIO class

/**
 * open and convert `filename` file to vector
 */
void FileIO::openAndConvert() {
  fileBuffer.open(fileName);
  if (fileBuffer.is_open()) {
    std::cout << "Load file success." << std::endl << std::endl;
    try {
      convertFileBufferToVector();
    } catch (const std::length_error &e) {
      std::cerr << e.what() << std::endl;  // if there was exceptions during the process
                                           // output it to std error;
    }
  } else {
    std::cerr << "can't open file name : " << fileName << std::endl;
    fileBuffer.close();  // if can not open file get a new filename and try again
    getFileName();
    openAndConvert();
  }
  fileBuffer.close();
}
// private methods

/**
 * tries to get the file name
 * @throws throw an exception when user enter EOF
 */
void FileIO::getFileName() {
  std::cout << "Please input the map file: ";
  std::cin >> fileName;
  if (std::cin.fail()) {
    handleCinError();
  }
}
/**
 * convert filebuffer to vector
 */
void FileIO::convertFileBufferToVector() {
  int input;
  while (fileBuffer >> input) {
    vectorOfInt.push_back(input);
  }
}
// partTwo class

/**
 * the main function for finding meaty, which follow the order of openfile -> create BST -> get user input -> find meaty
 */
void partTwo::findMeaty() {
  auto file = FileIO();
  file.openAndConvert();
  createBst(file.vectorOfInt);
  getSwordLocation();
  getMeatyLocation();
  getTrapIndex();
  std::vector<int> targetDeleteValue = findNodesToDelete();
  std::cout << std::endl;
  for (int i : targetDeleteValue) {
    try {
      partTwoBst.DeleteNumber(i);
    } catch (std::invalid_argument &e) {
      handleExceptionError(e);
      continue;
    }
    std::cout << "Number " << i << " is deleted." << std::endl;
  }
  std::cout << std::endl;
  try {
    findPathToSword(partTwoBst.getRoot(), false);
  } catch (std::logic_error &e) {
    std::cout << e.what() << std::endl;
    return;
  }
  std::cout << "Capoo successfully found his favorite meaty<3." << std::endl << std::endl;
  displayShortestPath();
}
// private methods

/**
 * create a BST with vector `vectorOfInt`
 * @param vectorOfInt the vector that we want to convert to BST
 */
void partTwo::createBst(std::vector<int> &vectorOfInt) {
  for (int i : vectorOfInt) {
    partTwoBst.InsertNumber(i);
  }
}
/**
 * get sword location through stdin
 */
void partTwo::getSwordLocation() {
  std::cout << "Please input the sword location: ";
  std::cin >> swordLocation;
}
/**
 * get meaty location through stdin
 */
void partTwo::getMeatyLocation() {
  std::cout << "Please input the Meaty's location: ";
  std::cin >> meatyLocation;
}
/**
 * get trap location through stdin
 */
void partTwo::getTrapIndex() {
  std::cout << "Please input the broccoli traps' index (0~9): ";
  std::cin >> trapIndex;
}
/**
 * since we want to delete all node that contain `trapIndex` we turn all node values to string and check if `trapIndex`
 * is a sub string node value
 * @param vectorOfInt the vector that contain all node values
 * @return a vector that contains all the nodes that fullfill the requirement of trapIndex
 */
std::vector<int> partTwo::findNodesToDelete() {
  std::vector<int> targetNodesValues;
  std::string target = std::to_string(trapIndex);
  traversBstInPostOrder(partTwoBst.getRoot(), target, targetNodesValues);
  return targetNodesValues;
}
/**
 * travers through the BST in postorder and push back node value to `targetNodesValues` vector when we find a match
 * @param nodeRoot the root of tree in the current function call
 * @param target the target string we want to find in node
 * @targetNodesValues the vector we want to push back when we find match
 */
void partTwo::traversBstInPostOrder(TreeNode *nodeRoot, std::string &target, std::vector<int> &targetNodesValues) {
  if (nodeRoot == nullptr) {
    return;
  }
  traversBstInPostOrder(nodeRoot->left, target, targetNodesValues);
  traversBstInPostOrder(nodeRoot->right, target, targetNodesValues);
  if (std::to_string(nodeRoot->value).find(target) != std::string::npos) {
    targetNodesValues.push_back(nodeRoot->value);
  }
  return;
}

/**
 * find the path to sword location according to the rule of BST, during the process, do things as listed 1. push node
 * pointer to `vectorOfPath` when visiting a new node. 2. check whether th path to sword location and meaty location
 * diffed on next  path, if it will be different, set deff to true and push node ptr to `stackOfTreeNodePtr` stack from
 * then on.
 * when found the sword call function `findNearestAnc` to find the nearest ancestor node then call function
 * `findPathToMeaty`
 * @param nodeRoot the root of tree in the current function call
 * @diff the flag that determine whether the path to sword and meaety have differed
 * @throws  throw error when we found dragon before getting the sword
 */
void partTwo::findPathToSword(TreeNode *nodeRoot, bool diff) {
  if (nodeRoot == nullptr) {
    return;
  }
  if (meatyLocation == nodeRoot->value && (meatyLocation != swordLocation)) {
    throw std::logic_error("found dragon before finding sword!");
  }
  vectorOfPath.push_back(nodeRoot->value);
  if (swordLocation == nodeRoot->value) {
    if (meatyLocation == swordLocation) {
      return;
    }
    TreeNode *nearestNode = findNearestAnc(nodeRoot);
    findPathToMeaty(nearestNode);
  } else if (swordLocation > nodeRoot->value) {
    if (diff) {
      stackOfTreeNodePtr.push(nodeRoot);
    } else {
      if (meatyLocation < nodeRoot->value) {
        diff = true;
        stackOfTreeNodePtr.push(nodeRoot);
      }
    }
    findPathToSword(nodeRoot->right, diff);
  } else {
    if (diff) {
      stackOfTreeNodePtr.push(nodeRoot);
    } else {
      if (meatyLocation > nodeRoot->value) {
        diff = true;
        stackOfTreeNodePtr.push(nodeRoot);
      }
    }
    findPathToSword(nodeRoot->left, diff);
  }
}
/**
 * follow path according to the `stackOfTreeNodePtr` stack, push node value to `vectorOfPath` while travering it.
 * @initNode the last node that we visited
 * @return the nearest ancestor
 */
TreeNode *partTwo::findNearestAnc(TreeNode *initNode) {
  TreeNode *top = nullptr;
  while (!stackOfTreeNodePtr.empty()) {
    top = stackOfTreeNodePtr.top();
    stackOfTreeNodePtr.pop();
    vectorOfPath.push_back(top->value);
  }
  if (top == nullptr) {
    return initNode;
  }
  return top;
}
/**
 * similer to the searchNumber method in BST class but we have to push path value to `vectorOfPath` to record the path
 * we  visited
 * @param nodeRoot the root of tree in the current function call
 */
void partTwo::findPathToMeaty(TreeNode *nodeRoot) {
  if (nodeRoot == nullptr) {
    return;
  }
  if (meatyLocation == nodeRoot->value) {
    return;
  }
  if (meatyLocation > nodeRoot->value) {
    vectorOfPath.push_back(nodeRoot->right->value);
    findPathToMeaty(nodeRoot->right);
  } else {
    vectorOfPath.push_back(nodeRoot->left->value);
    findPathToMeaty(nodeRoot->left);
  }
}
/**
 * display the shortest to meaty
 */
void partTwo::displayShortestPath() {
  std::cout << "Shortest path to find the meaty :" << std::endl;
  std::cout << vectorOfPath.at(0);
  for (size_t i = 1; i < vectorOfPath.size(); i++) {
    std::cout << "->" << vectorOfPath.at(i);
  }
  std::cout << std::endl;
}
