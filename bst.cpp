#include <cctype>
#include <exception>
#include <fstream>
#include <iostream>
#include <queue>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <string>
#include <vector>
struct TreeNode {
  int value;
  TreeNode *left;
  TreeNode *right;
  TreeNode() : value(0), left(nullptr), right(nullptr) {}
  TreeNode(int x) : value(x), left(nullptr), right(nullptr) {}
  TreeNode(int x, TreeNode *left, TreeNode *right) : value(x), left(left), right(right) {}
};
class BST {
 public:
  enum printType { prefix, infix, postfix, level };

  BST() : root(nullptr){};
  BST(int x) { root = new TreeNode(x); };
  bool InsertNumber(int number);
  bool DeleteNumber(int targetValue);
  bool SearchNumber(int targetValue);
  void PrintALL();
  TreeNode *getRoot() { return root; }

 private:
  TreeNode *root;
  bool deleted;

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
  void display();
  char getUserInput();
  mainMenuOptions parseUserInput();

 private:
  char input;
};
class partOne {
 public:
  enum partOneOptions { I, D, S, P, R, error };

  partOne() : partOneBst(BST()) {}
  int loop();
  void display();
  void getUserInput();
  partOneOptions parseUserInput();

 private:
  char input;
  BST partOneBst;
};
class FileIO {
 public:
  std::vector<int> vectorOfInt;
  std::string fileName;
  std::ifstream fileBuffer;
  FileIO() { getFileName(); }  // try to get the filename at initialization

  void openAndConvert();

 private:
  /**
   * tries to get the file name
   * @throws throw an exception when user enter EOF
   */
  void getFileName();
  void convertStringToVector();
};

class partTwo {
 public:
  partTwo() : partTwoBst(BST()) {}
  void findMeaty();

 private:
  BST partTwoBst;
  int swordLocation;
  int meatyLocation;
  int trapIndex;
  std::stack<TreeNode *> stackOfTreeNodePtr;
  std::vector<int> vectorOfPath;

  void createBst(std::vector<int> vectorOfInt);
  void getSwordLocation();
  void getMeatyLocation();
  void getTrapIndex();
  std::vector<int> findNodesToDelete(std::vector<int> vectorOfInt);
  void findPathToSword(TreeNode *nodeRoot, bool diff);
  TreeNode *findNearestAnc(TreeNode *initNode);
  void findPathToMeaty(TreeNode *nodeRoot);
  void displayShortestPath();
};
int main() {
  mainMenu menu = mainMenu();
  menu.loop();
}
void printInvalidOptionError() { std::cerr << "Invalid input, please enter valid value" << std::endl; }
std::vector<int> parseMultipleInput() {
  int number;
  std::vector<int> vectorOfInt;
  while (!std::cin.fail()) {
    std::cin >> number;
    if (number == -1) {
      break;
    }
    if (!std::cin.fail()) {
      vectorOfInt.push_back(number);
    }
  }
  return vectorOfInt;
}

bool BST::InsertNumber(int number) {
  try {
    root = insertNode(root, number);
  } catch (const std::invalid_argument &e) {
    throw;
    return false;
  }
  return true;
}
bool BST::DeleteNumber(int targetValue) {
  deleted = false;
  root = deleteNode(root, targetValue);
  if (deleted == false) {
    std::string errorString;
    errorString = "Number " + std::to_string(targetValue) + " is not exists.";
    throw std::invalid_argument(errorString);
  }
  return deleted;
}
bool BST::SearchNumber(int targetValue) {
  bool result = searchNode(root, targetValue);
  if (result == false) {
    std::string errorString;
    errorString = "SORRY. " + std::to_string(targetValue) + " is not found.";
    throw std::invalid_argument(errorString);
  }
  return result;
}
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
TreeNode *BST::insertNode(TreeNode *nodeRoot, int number) {
  if (nodeRoot == nullptr) {
    TreeNode *newNode = new TreeNode(number);
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
int mainMenu::loop() {
  while (1) {
    display();
    try {
      getUserInput();
    } catch (std::invalid_argument &e) {
      std::cerr << e.what() << std::endl;
      return 0;
    }
    switch (parseUserInput()) {
      case first: {
        partOne partone = partOne();
        partone.loop();
        break;
      }
      case second: {
        partTwo parttwo = partTwo();
        parttwo.findMeaty();
        break;
      }
      case zero: {
        return 0;
      }
      case error: {
        printInvalidOptionError();
        break;
      }
    }
  }
}
void mainMenu::display() {
  std::cout << "(1)Binary searching Tree." << std::endl;
  std::cout << "(2)Finding Meaty." << std::endl;
  std::cout << "(0)Escape and face to music next year." << std::endl;
}
char mainMenu::getUserInput() {
  std::cin >> input;
  if (std::cin.fail()) {
    throw std::invalid_argument("entered invalid input");
  }
  return input;
}
mainMenu::mainMenuOptions mainMenu::parseUserInput() {
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
int partOne::loop() {
  while (1) {
    display();
    try {
      getUserInput();
    } catch (std::invalid_argument &e) {
      return 0;
    }
    switch (parseUserInput()) {
      case I: {
        std::cout << "Enter numbers: ";
        std::vector<int> vectorOfInt = parseMultipleInput();
        for (int i : vectorOfInt) {
          try {
            partOneBst.InsertNumber(i);
          } catch (std::invalid_argument &e) {
            std::cerr << e.what() << std::endl;
            continue;
          }
          std::cout << "Number " << i << " is inserted." << std::endl;
        }
        break;
      }
      case D: {
        std::cout << "Enter numbers to deleted : ";
        std::vector<int> vectorOfInt = parseMultipleInput();
        for (int i : vectorOfInt) {
          try {
            partOneBst.DeleteNumber(i);
          } catch (std::invalid_argument &e) {
            std::cerr << e.what() << std::endl;
            continue;
          }
          std::cout << "Number " << i << " is deleted." << std::endl;
        }
        break;
      }
      case S: {
        std::cout << "Enter elements to searching : ";
        std::vector<int> vectorOfInt = parseMultipleInput();
        for (int i : vectorOfInt) {
          try {
            partOneBst.SearchNumber(i);
          } catch (std::invalid_argument &e) {
            std::cerr << e.what() << std::endl;
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
      case R: {
        return 0;
      }
    }
  }
}
void partOne::display() {
  std::cout << "(I)nsert a number." << std::endl;
  std::cout << "(D)elete a number." << std::endl;
  std::cout << "(S)earch a number." << std::endl;
  std::cout << "(P)rint 4 kinds of order." << std::endl;
  std::cout << "(R)eturn" << std::endl;
}
void partOne::getUserInput() {
  std::cin >> input;
  if (std::cin.fail()) {
    throw std::invalid_argument("entered invalid input");
  }
}
partOne::partOneOptions partOne::parseUserInput() {
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
void FileIO::openAndConvert() {
  fileBuffer.open(fileName);
  if (fileBuffer.is_open()) {
    std::cout << "Load file success." << std::endl << std::endl;
    try {
      convertStringToVector();
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
void FileIO::getFileName() {
  std::cout << "Please input the map file: ";
  std::cin >> fileName;
  if (std::cin.fail()) {
    throw std::invalid_argument("entered invalid input");
  }
}
void FileIO::convertStringToVector() {
  int input;
  while (fileBuffer >> input) {
    vectorOfInt.push_back(input);
  }
}
void partTwo::findMeaty() {
  FileIO file = FileIO();
  file.openAndConvert();
  createBst(file.vectorOfInt);
  getSwordLocation();
  getMeatyLocation();
  getTrapIndex();
  std::vector<int> targetDeleteValue = findNodesToDelete(file.vectorOfInt);
  std::cout << std::endl;
  for (int i : targetDeleteValue) {
    try {
      partTwoBst.DeleteNumber(i);
    } catch (std::invalid_argument &e) {
      std::cerr << e.what() << std::endl;
      continue;
    }
    std::cout << "Number " << i << " is deleted." << std::endl;
  }
  std::cout << std::endl;
  try {
    findPathToSword(partTwoBst.getRoot(), false);
  } catch (std::exception &e) {
    std::cout << e.what() << std::endl;
    return;
  }
  std::cout << "Capoo successfully found his favorite meaty<3." << std::endl << std::endl;
  displayShortestPath();
}
void partTwo::createBst(std::vector<int> vectorOfInt) {
  for (int i : vectorOfInt) {
    partTwoBst.InsertNumber(i);
  }
}
void partTwo::getSwordLocation() {
  std::cout << "Please input the sword location: ";
  std::cin >> swordLocation;
}

void partTwo::getMeatyLocation() {
  std::cout << "Please input the Meaty's location: ";
  std::cin >> meatyLocation;
}
void partTwo::getTrapIndex() {
  std::cout << "Please input the broccoli traps' index (0~9): ";
  std::cin >> trapIndex;
}
std::vector<int> partTwo::findNodesToDelete(std::vector<int> vectorOfInt) {
  std::vector<int> targetNodesValues;
  std::string target = std::to_string(trapIndex);
  for (int i : vectorOfInt) {
    std::string j = std::to_string(i);
    if (j.find(target) != std::string::npos) {
      targetNodesValues.push_back(i);
    }
  }
  return targetNodesValues;
}

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
void partTwo::displayShortestPath() {
  std::cout << "Shortest path to find the meaty :" << std::endl;
  std::cout << vectorOfPath.at(0);
  for (size_t i = 1; i < vectorOfPath.size(); i++) {
    std::cout << "->" << vectorOfPath.at(i);
  }
  std::cout << std::endl;
}
