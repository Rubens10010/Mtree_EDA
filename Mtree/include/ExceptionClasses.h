#ifndef EXCEPTIONCLASSES_H
#define EXCEPTIONCLASSES_H
//#include "M_node.h"

class Data;
class M_node;
class SplitNodeReplace
{
public:
    enum {NUM_NODES = 2};
    M_node* newNodes[NUM_NODES];
    SplitNodeReplace(M_node* newNodes[NUM_NODES])
    {
      for(int i=0; i<NUM_NODES; ++i)
      {
          this->newNodes[i] = newNodes[i];
      }
    }
};

class RootNodeReplacement
{
public:
    M_node* newRoot;
};

class NodeUnderCapacity{};

class DataNotFound{
  public:
      Data data;
};

#endif EXCEPTIONCLASSES_H
