#ifndef BINARYTREES_H
#define BINARYTREES_H

#include <stack>
#include <vector>

template<typename T>
concept KEY = requires(const T & t1, const T & t2)
{
	t1 > t2;
	t1 == t2;
	t1 < t2;
};

enum class TREE_TYPES
{
	RANDOMIZED,
	AVL,
	RB
};

template<KEY KeyType, typename ValueType>
struct BasicNode
{
	KeyType key;
	ValueType value;

	BasicNode* parent;
	BasicNode* left;
	BasicNode* right;

	BasicNode(const KeyType& _key, const ValueType& _value, BasicNode* _parent = nullptr)
		: key(_key), value(_value), parent(_parent), left(nullptr), right(nullptr) {};

	virtual ~BasicNode() {};

	virtual short getHeight() { return -1; }
	virtual char getColor() { return 0; }
	virtual void setHeight(short _height) { return; }
	virtual void setColor(char _color) { return; }
};

template<KEY KeyType, typename ValueType>
struct AVLNode : public BasicNode<KeyType, ValueType>
{	
	short height;
	
	AVLNode(const KeyType& _key, const ValueType& _value, BasicNode<KeyType, ValueType>* _parent = nullptr) :
		BasicNode<KeyType, ValueType>(_key, _value, _parent), height(0) {};
	virtual ~AVLNode() {};

	virtual short getHeight() override { return height; };
	virtual void setHeight(short _height) override { height = _height; };
};

template<KEY KeyType, typename ValueType>
struct RBNode : public BasicNode<KeyType, ValueType>
{
	char color;

	RBNode(const KeyType& _key, const ValueType& _value, BasicNode<KeyType, ValueType>* _parent = nullptr) :
		BasicNode<KeyType, ValueType>(_key, _value, _parent), color('R') {};
	virtual ~RBNode() {};

	virtual char getColor() override { return color; }
	virtual void setColor(char _color) override { color = _color; }
};


//------------------------------------------------------------------------------------------------------
//--------------------------------------------- CLASS TREE ---------------------------------------------
//------------------------------------------------ BEGIN -----------------------------------------------

template<KEY KeyType, typename ValueType>
class Tree
{
	using Node = BasicNode<KeyType, ValueType>;

//Public structs:	
public:
	struct Iterator
	{
	private:
		Node* pointerToNode;
		Tree<KeyType,ValueType>* pointerToOwner;
	public:
		Iterator(Node* _ptr, Tree<KeyType, ValueType>* _owner) :
			pointerToNode(_ptr), pointerToOwner(_owner) {};

		std::pair<KeyType, ValueType&> operator*() const
		{
			return { pointerToNode->key, pointerToNode->value };
		}

		friend bool operator==(const Iterator& _it1, const Iterator& _it2)
		{
			return _it1.pointerToNode == _it2.pointerToNode;
		}

		void operator++()
		{	
			if (*this == pointerToOwner->afterEnd())
				return;

			if (*this == pointerToOwner->end())
			{
				*this = pointerToOwner->afterEnd();
				return;
			}

			if (*this == pointerToOwner->beforeBegin())
			{
				*this = pointerToOwner->begin();
				return;
			}

			Node* searchPtr;

			if (pointerToNode->right)
			{
				searchPtr = pointerToNode->right;
				while (searchPtr->left)
					searchPtr = searchPtr->left;

				pointerToNode = searchPtr;
				return;
			}

			searchPtr = pointerToNode->parent;
			while (searchPtr->key < pointerToNode->key)
				searchPtr = searchPtr->parent;

			pointerToNode = searchPtr;
		}

		void operator--()
		{
			if (*this == pointerToOwner->beforeBegin())
				return;

			if (*this == pointerToOwner->begin())
			{
				*this = pointerToOwner->beforeBegin();
				return;
			}

			if (*this == pointerToOwner->afterEnd())
			{
				*this = pointerToOwner->end();
				return;
			}

			Node* searchPtr;

			if (pointerToNode->left)
			{
				searchPtr = pointerToNode->left;
				while (searchPtr->right)
					searchPtr = searchPtr->right;

				pointerToNode = searchPtr;
				return;
			}

			searchPtr = pointerToNode->parent;
			while (searchPtr->key > pointerToNode->parent)
				searchPtr = searchPtr->parent;

			pointerToNode = searchPtr;
		}
	};

//Protected members:
protected:	
	Node* root;
	Node* last_added_node;
	Node* parent_of_last_erased_node;

	int m_size;

	void swapNodes(Node* _node1, Node* _node2);
	Node* innerFind(const KeyType& _key);

//Public members:
public:
	Tree() : root(nullptr), last_added_node(nullptr), parent_of_last_erased_node(nullptr), m_size(0) {};
	Tree(const KeyType& _key, const ValueType& _value) : root(new Node(_key, _value)), last_added_node(root), m_size(1) {};
	Tree(const std::pair<KeyType, ValueType>& _pair) : Tree(_pair.first, _pair.second) {};
	Tree(const std::vector<std::pair<KeyType, ValueType>>& _vector)
	{
		Tree();
		for (const auto& pair : _vector)
			insert(pair);
	};
	
	virtual ~Tree() { clear();};

	bool empty() const { return (!root) ? true : false; };
	int size() const { return m_size; };

	virtual bool insert(const KeyType& _key, const ValueType& _value);
	bool insert(const std::pair<KeyType, ValueType>& _pair);

	virtual bool erase(const KeyType& _key);
	bool erase(Iterator& _iterator);

	bool setValue(const KeyType& _key, const ValueType& _value);

	std::pair<bool, Iterator> find(const KeyType& _key);
	std::vector< std::pair<KeyType, ValueType&> > getVector() const;

	Iterator begin();
	Iterator end(); 
	Iterator beforeBegin();
	Iterator afterEnd();

	void clear();
};

template<KEY KeyType, typename ValueType>
void Tree<KeyType, ValueType>::swapNodes(Node* _node1, Node* _node2)
{
	Node temp = *_node1;
	_node1->key = _node2->key;
	_node1->value = _node2->value;

	_node2->key = temp.key;
	_node2->value = temp.value;
}

template<KEY KeyType, typename ValueType>
Tree<KeyType, ValueType>::Node* Tree<KeyType, ValueType>::innerFind(const KeyType& _key)
{
	if (!root)
		return nullptr;

	Node* searchPtr = root;
	while(true)
	{
		if (_key == searchPtr->key)
			return searchPtr;

		if (_key > searchPtr->key)
		{
			if (!searchPtr->right)
				return nullptr;

			searchPtr = searchPtr->right;
		}
		else
		{
			if (!searchPtr->left)
				return nullptr;

			searchPtr = searchPtr->left;
		}
	}
}

//template<KEY KeyType, typename ValueType>
//bool Tree<KeyType, ValueType>::insert(const KeyType& _key, const ValueType& _value)
//{
//	if (!root)
//	{
//		switch (type)
//		{
//		case TREE_TYPES::RANDOMIZED:
//			root = new BasicNode<KeyType, ValueType>(_key, _value);
//			break;
//		case TREE_TYPES::AVL:
//			root = new AVLNode<KeyType, ValueType>(_key, _value);
//			break;
//		case TREE_TYPES::RB:
//			root = new RBNode<KeyType, ValueType>(_key, _value);
//			break;
//		}
//
//		last_added_node = root;
//		++m_size;
//		return true;
//	}
//
//	Node* searchPtr = root;
//	while (true)
//	{
//		if (_key == searchPtr->key)
//			return false;
//
//		if (_key > searchPtr->key)
//		{
//			if (!searchPtr->right)
//			{
//				switch (type)
//				{
//				case TREE_TYPES::RANDOMIZED:
//					searchPtr->right = new BasicNode<KeyType, ValueType>(_key, _value, searchPtr);
//					break;
//				case TREE_TYPES::AVL:
//					searchPtr->right = new AVLNode<KeyType, ValueType>(_key, _value, searchPtr);
//					break;
//				case TREE_TYPES::RB:
//					searchPtr->right = new RBNode<KeyType, ValueType>(_key, _value, searchPtr);
//					break;
//				}
//
//				last_added_node = searchPtr->right;
//				++m_size;				
//				return true;
//			}
//
//			searchPtr = searchPtr->right;
//		}
//		else
//		{
//			if (!searchPtr->left)
//			{
//				switch (type)
//				{
//				case TREE_TYPES::RANDOMIZED:
//					searchPtr->left = new BasicNode<KeyType, ValueType>(_key, _value, searchPtr);
//					break;
//				case TREE_TYPES::AVL:
//					searchPtr->left = new AVLNode<KeyType, ValueType>(_key, _value, searchPtr);
//					break;
//				case TREE_TYPES::RB:
//					searchPtr->left = new RBNode<KeyType, ValueType>(_key, _value, searchPtr);
//					break;
//				}
//
//				last_added_node = searchPtr->left;
//				++m_size;
//				return true;
//			}
//
//			searchPtr = searchPtr->left;
//		}
//	}
//}

template<KEY KeyType, typename ValueType>
bool Tree<KeyType, ValueType>::insert(const KeyType& _key, const ValueType& _value)
{
	if (!root)
	{
		root = new Node(_key, _value);
		last_added_node = root;
		++m_size;
		return true;
	}

	Node* searchPtr = root;

	while (true)
	{
		if (_key == searchPtr->key)
			return false;

		if (_key > searchPtr->key)
		{
			if (!searchPtr->right)
			{
				searchPtr->right = new Node(_key, _value, searchPtr);
				last_added_node = searchPtr->right;
				++m_size;
				return true;
			}

			searchPtr = searchPtr->right;
		}
		else
		{
			if (!searchPtr->left)
			{
				searchPtr->left = new Node(_key, _value, searchPtr);
				last_added_node = searchPtr->left;
				++m_size;
				return true;
			}
			searchPtr = searchPtr->left;
		}
	}

}

template<KEY KeyType, typename ValueType>
bool Tree<KeyType, ValueType>::insert(const std::pair<KeyType, ValueType>& _pair)
{
	return insert(_pair.first, _pair.second);
}

template <KEY KeyType, typename ValueType>
bool Tree<KeyType, ValueType>::erase(const KeyType& _key)
{
	Node* nodeToErase = innerFind(_key);
	
	if (!nodeToErase)
		return false;

	//≈сли у удал€емого узла 2 потомка, ищем самый правый узел левого поддерева
	if (nodeToErase->left && nodeToErase->right)
	{
		Node* newNode = nodeToErase->left;
		while (newNode->right)
			newNode = newNode->right;

		//—вапаем ключ и значение узлов
		swapNodes(nodeToErase, newNode);
		parent_of_last_erased_node = newNode->parent;

		//≈сли у удал€емого узла есть левый потомок, переписываем указатели у него и родител€
		if (newNode->left)
		{
			Node* leftChild = newNode->left;
			leftChild->parent = parent_of_last_erased_node;

			if (parent_of_last_erased_node->right == newNode)
				parent_of_last_erased_node->right = leftChild;
			else
				parent_of_last_erased_node->left = leftChild;

			//‘актически удал€ем узел
			delete newNode;
			--m_size;
			return true;
		}

		//≈сли левого потомка нет, просто удал€ем узел
		if (parent_of_last_erased_node->right == newNode)
			parent_of_last_erased_node->right = nullptr;
		else
			parent_of_last_erased_node->left = nullptr;

		delete newNode;
		--m_size;
		return true;
	}

	//≈сли у удал€емого узла есть только один потомок - левый
	if (nodeToErase->left)
	{
		if (nodeToErase == root)
		{
			root = root->left;
			root->parent = nullptr;
			parent_of_last_erased_node = root;

			delete nodeToErase;
			--m_size;
			return true;
		}
		
		parent_of_last_erased_node = nodeToErase->parent;
		Node* leftChild = nodeToErase->left;
		leftChild->parent = parent_of_last_erased_node;
		
		if (parent_of_last_erased_node->right == nodeToErase)
			parent_of_last_erased_node->right = leftChild;
		else
			parent_of_last_erased_node->left = leftChild;

		delete nodeToErase;
		--m_size;
		return true;
	}

	//≈сли у удал€емого узла есть только один потомок - правый
	if (nodeToErase->right)
	{
		if (nodeToErase == root)
		{
			root = root->right;
			root->parent = nullptr;
			parent_of_last_erased_node = root;

			delete nodeToErase;
			--m_size;
			return true;
		}

		parent_of_last_erased_node = nodeToErase->parent;
		Node* rightChild = nodeToErase->right;
		rightChild->parent = parent_of_last_erased_node;

		if (parent_of_last_erased_node->right == nodeToErase)
			parent_of_last_erased_node->right = rightChild;
		else
			parent_of_last_erased_node->left = rightChild;

		delete nodeToErase;
		--m_size;
		return true;
	}

	//≈сли потомков не было вообще
	parent_of_last_erased_node = nodeToErase->parent;
	
	if (parent_of_last_erased_node)
	{

		if (parent_of_last_erased_node->right == nodeToErase)
			parent_of_last_erased_node->right = nullptr;
		else
			parent_of_last_erased_node->left = nullptr;
	}
	else
		root = nullptr;

	delete nodeToErase;
	--m_size;
	return true;
}

template<KEY KeyType, typename ValueType>
bool Tree<KeyType, ValueType>::erase(Iterator& _iterator)
{
	KeyType _key = (*_iterator).first;
	_iterator = { nullptr, nullptr };

	return erase(_key);
}

template<KEY KeyType, typename ValueType>
bool Tree<KeyType, ValueType>::setValue(const KeyType& _key, const ValueType& _value)
{
	Node* result = innerFind(_key);
	if (!result)
		return false;

	result->value = _value;
	return true;
}

template<KEY KeyType, typename ValueType>
std::pair<bool, typename Tree<KeyType, ValueType>::Iterator> Tree<KeyType, ValueType>::find(const KeyType& _key)
{
	Node* result = innerFind(_key);
	if (!result)
		return { false, {nullptr, nullptr} };

	return { true, {result, this} };
}

template<KEY KeyType, typename ValueType>
std::vector< std::pair<KeyType, ValueType&> > Tree<KeyType, ValueType>::getVector() const
{
	std::vector< std::pair<KeyType, ValueType&> > vector;
	vector.reserve(m_size);
	std::stack<Node*> stack;
	stack.push(root);

	Node* searchPtr = root->left;
	while (!stack.empty() || searchPtr)
	{
		while (searchPtr)
		{
			stack.push(searchPtr);
			searchPtr = searchPtr->left;
		}

		searchPtr = stack.top();
		stack.pop();

		vector.push_back({ searchPtr->key, searchPtr->value });
		searchPtr = searchPtr->right;
	}

	return vector;
}

template<KEY KeyType, typename ValueType>
Tree<KeyType, ValueType>::Iterator  Tree<KeyType, ValueType>::begin()
{
	if (!root)
		return { nullptr, nullptr };

	Node* searchPtr = root;
	while (searchPtr->left)
		searchPtr = searchPtr->left;

	return { searchPtr, this };
}

template<KEY KeyType, typename ValueType>
Tree<KeyType, ValueType>::Iterator  Tree<KeyType, ValueType>::beforeBegin()
{
	if (!root)
		return { nullptr, nullptr };

	Node* searchPtr = root;
	while (searchPtr->left)
		searchPtr = searchPtr->left;

	return { searchPtr - 1, this };
}

template<KEY KeyType, typename ValueType>
Tree<KeyType, ValueType>::Iterator  Tree<KeyType, ValueType>::end() 
{
	if (!root)
		return { nullptr, nullptr };

	Node* searchPtr = root;
	while (searchPtr->right)
		searchPtr = searchPtr->right;

	return { searchPtr, this };
}

template<KEY KeyType, typename ValueType>
Tree<KeyType, ValueType>::Iterator  Tree<KeyType, ValueType>::afterEnd() 
{
	if (!root)
		return { nullptr, nullptr };

	Node* searchPtr = root;
	while (searchPtr->right)
		searchPtr = searchPtr->right;

	return { searchPtr + 1, this };
}

template<KEY KeyType, typename ValueType>
void Tree<KeyType, ValueType>::clear()
{
	if (!root)
		return;

	std::stack<Node*> stack;
	stack.push(root);

	Node* searchPtr = root->left;
	Node* deletePtr;

	while (!stack.empty() || searchPtr)
	{
		while (searchPtr)
		{
			stack.push(searchPtr);
			searchPtr = searchPtr->left;
		}

		searchPtr = deletePtr = stack.top();
		stack.pop();
		searchPtr = searchPtr->right;

		delete deletePtr;
		--m_size;
	}

	root = nullptr;
}

//------------------------------------------------------------------------------------------------------
//--------------------------------------------- CLASS TREE ---------------------------------------------
//------------------------------------------------- END ------------------------------------------------



//------------------------------------------------------------------------------------------------------
//-------------------------------------------- CLASS AVLTREE -------------------------------------------
//------------------------------------------------ BEGIN -----------------------------------------------

template<KEY KeyType, typename ValueType>
class AVLTree : public Tree<KeyType, ValueType>
{
	using Node = BasicNode<KeyType, ValueType>;

//Protected members:
protected:
	void calculateHeight(Node* _node);
	void updateHeight(Node* _node);
	void getSubTreesHeight(Node* _node, short& lheight, short& rheight);

	void leftRotate(Node* _node);
	void rightRotate(Node* _node);
	void balance(Node* _node);

//Public members:
public:
	AVLTree() {};
	virtual ~AVLTree() {};

	virtual bool insert(const KeyType& _key, const ValueType& _value) override;
	virtual bool erase(const KeyType& _key) override;

	bool isBalanced(int& _key)
	{
		Node* searchPtr = this->root;
		std::stack<Node*> stack;
		stack.push(searchPtr);
		searchPtr = searchPtr->left;

		while (!stack.empty() || searchPtr)
		{
			while (searchPtr)
			{
				stack.push(searchPtr);
				searchPtr = searchPtr->left;
			}

			searchPtr = stack.top();
			stack.pop();

			int lheight = -1;
			int rheight = -1;

			if (searchPtr->left)
				lheight = searchPtr->left->getHeight();
			if (searchPtr->right)
				rheight = searchPtr->right->getHeight();

			if ((rheight)-(lheight) <= -2)
			{
				_key = searchPtr->key;
				return false;
			}

			if ((rheight)-(lheight) >= 2)
			{
				_key = searchPtr->key;
				return false;
			}

			searchPtr = searchPtr->right;
		}
		return true;
	}
};

template<KEY KeyType, typename ValueType>
void AVLTree<KeyType, ValueType>::calculateHeight(Node* _node)
{
	short lheight = -1;
	short rheight = -1;
	getSubTreesHeight(_node, lheight, rheight);

	_node->setHeight(std::max(lheight, rheight) + 1);
}

template<KEY KeyType, typename ValueType>
void AVLTree<KeyType, ValueType>::updateHeight(Node* _node)
{
	while (_node)
	{
		calculateHeight(_node);
		_node = _node->parent;
	}
}

template<KEY KeyType, typename ValueType>
void AVLTree<KeyType, ValueType>::getSubTreesHeight(Node* _node, short& lheight, short& rheight)
{
	if (!_node)
		return;

	if (_node->left)
		lheight = _node->left->getHeight();
	if (_node->right)
		rheight = _node->right->getHeight();
}

template<KEY KeyType, typename ValueType>
void AVLTree<KeyType, ValueType>::leftRotate(Node* _node)
{
	Node* parentOfNode = _node->parent;
	Node* rightChild = _node->right;

	short lheight = -1;
	short rheight = -1;
	getSubTreesHeight(rightChild, lheight, rheight);

	//≈сли высота левого поддерева правого ребенка больше высоты его правого поддерева
	//делаем большой левый поворот
	if (lheight > rheight)
	{
		rightRotate(rightChild);
		leftRotate(_node);
		return;
	}

	//¬ противном случае совершаем простой левый поворот
	_node->right = rightChild->left;
	if (_node->right)
		_node->right->parent = _node;
	calculateHeight(_node);

	rightChild->left = _node;
	_node->parent = rightChild;
	calculateHeight(rightChild);

	if (!parentOfNode)
	{
		this->root = rightChild;
		this->root->parent = nullptr;
		return;
	}

	if (parentOfNode->right == _node)
		parentOfNode->right = rightChild;
	else
		parentOfNode->left = rightChild;

	rightChild->parent = parentOfNode;
	updateHeight(parentOfNode);
}

template<KEY KeyType, typename ValueType>
void AVLTree<KeyType, ValueType>::rightRotate(Node* _node)
{
	Node* parentOfNode = _node->parent;
	Node* leftChild = _node->left;

	short lheight = -1;
	short rheight = -1;
	getSubTreesHeight(leftChild, lheight, rheight);

	//≈сли высота левого поддерева правого ребенка больше высоты его правого поддерева
	//делаем большой левый поворот
	if (rheight > lheight)
	{
		leftRotate(leftChild);
		rightRotate(_node);
		return;
	}
	
	//¬ противном случаем делаем простой правый поворот
	_node->left = leftChild->right;
	if (_node->left)
		_node->left->parent = _node;
	calculateHeight(_node);

	leftChild->right = _node;
	_node->parent = leftChild;
	calculateHeight(leftChild);

	if (!parentOfNode)
	{
		this->root = leftChild;
		this->root->parent = nullptr;
		return;
	}

	if (parentOfNode->right == _node)
		parentOfNode->right = leftChild;
	else
		parentOfNode->left = leftChild;

	leftChild->parent = parentOfNode;
	updateHeight(parentOfNode);
}
	
template<KEY KeyType, typename ValueType>
void AVLTree<KeyType, ValueType>::balance(Node* _node)
{
	while (_node)
	{
		short lheight = -1;
		short rheight = -1;
		getSubTreesHeight(_node, lheight, rheight);

		// ≈сли поддерево перегружено влево
		if ((rheight)-(lheight) <= -2)
		{
			rightRotate(_node);
			return;
		}
		
		// ≈сли поддерево перегружено вправо
		if ((rheight)-(lheight) >= 2)
		{
			leftRotate(_node);
			return;
		}

		_node = _node->parent;
	}
}

template<KEY KeyType, typename ValueType>
bool AVLTree<KeyType, ValueType>::insert(const KeyType& _key, const ValueType& _value)
{
	if (!this->root)
	{
		this->root = new AVLNode(_key, _value);
		++(this->m_size);
		return true;
	}

	Node* searchPtr = this->root;

	while (true)
	{
		if (_key == searchPtr->key)
			return false;

		if (_key > searchPtr->key)
		{
			if (!searchPtr->right)
			{
				searchPtr->right = new Node(_key, _value, searchPtr);
				this->last_added_node = searchPtr->right;
				++(this->m_size);
				break;
			}

			searchPtr = searchPtr->right;
		}
		else
		{
			if (!searchPtr->left)
			{
				searchPtr->left = new Node(_key, _value, searchPtr);
				this->last_added_node = searchPtr->left;
				++(this->m_size);
				break;
			}
			searchPtr = searchPtr->left;
		}
	}

	//≈сли у родительского узла стало 2 ребенка, 
	//значит высота не изменилась и балансировка не нужна
	if (this->last_added_node->parent->left &&
		this->last_added_node->parent->right) 
		return true;

	updateHeight(this->last_added_node);
	balance(this->last_added_node->parent);

	return true;
}

template<KEY KeyType, typename ValueType>
bool AVLTree<KeyType, ValueType>::erase(const KeyType& _key)
{
	bool result = Tree<KeyType, ValueType>::erase(_key);
	if (!result)
		return false;

	if (!this->parent_of_last_erased_node)
		return true;

	updateHeight(this->parent_of_last_erased_node);
	balance(this->parent_of_last_erased_node);
	return true;
}

//------------------------------------------------------------------------------------------------------
//-------------------------------------------- CLASS AVLTREE -------------------------------------------
//------------------------------------------------- END ------------------------------------------------



//------------------------------------------------------------------------------------------------------
//-------------------------------------------- CLASS RBTREE --------------------------------------------
//----------------------------------------------- BEGIN ------------------------------------------------
template<KEY KeyType, typename ValueType>
class RBTree : public Tree<KeyType, ValueType>
{
	using Node = BasicNode<KeyType, ValueType>;
protected:

	struct structForInsertBalance
	{
		Node* father; Node* grand_father; Node* uncle;
		char f_color; char gf_color; char u_color;
		char node_side; char father_side;

		structForInsertBalance(Node* _node)
		{
			setNode(_node);
		}

		void setNode(Node* _node)
		{
			father = _node->parent;
			f_color = father->getColor();

			if (_node == father->right)
				node_side = 'R';
			else node_side = 'L';

			grand_father = father->parent;
			gf_color = grand_father->getColor();

			if (father == grand_father->right)
			{
				father_side = 'R';
				uncle = grand_father->left;
			}
			else
			{
				father_side = 'L';
				uncle = grand_father->right;
			}

			if (uncle)
				u_color = uncle->getColor();
			else
				u_color = 'B';
		}
	};

	struct structForEraseBalance
	{
		Node* parent; Node* brother; Node* new_node;
		char node_color; char b_color; char node_side; char child_count;
		char left_nephew_color; char right_nephew_color;

		structForEraseBalance() {};
		structForEraseBalance(Node* _node)
		{
			setNode(_node);
		}

		setNode(Node* _node)
		{
			parent = _node->parent;
			node_color = _node->getColor();
			if (_node == parent->right)
			{
				node_side = 'R';
				brother = parent->left;
			}
			else
			{
				node_side = 'L';
				brother = parent->right;
			}

			if (brother)
			{
				b_color = brother->getColor();
				if (brother->right)
					right_nephew_color = brother->right->getColor();
				else
					right_nephew_color = 'B';

				if (brother->left)
					left_nephew_color = brother->left->getColor();
				else
					left_nephew_color = 'B';
			}
			else
				b_color = left_nephew_color = right_nephew_color = 'B';
			
			if (_node->right)
			{
				child_count = 1;
				new_node = _node->right;
			}
			else if (_node->left)
			{
				child_count = 1;
				new_node = _node->left;
			}
			else
			{
				child_count = 0;
				new_node = nullptr;
			}
		}
	};

	void leftRotate(Node* _node);
	void rightRotate(Node* _node);

	void insertBalance();
	void eraseBalance(structForEraseBalance& sfeb);


public:
	RBTree() {};

	bool insert(const KeyType& _key, const ValueType& _value);

	bool erase(const KeyType& _key);
};

template<KEY KeyType, typename ValueType>
void RBTree<KeyType, ValueType>::leftRotate(Node* _node)
{
	Node* parentOfNode = _node->parent;
	Node* rightChild = _node->right;

	_node->right = rightChild->left;
	if (_node->right)
		_node->right->parent = _node;

	rightChild->left = _node;
	_node->parent = rightChild;

	if (!parentOfNode)
	{
		this->root = rightChild;
		this->root->parent = nullptr;
		return;
	}

	if (parentOfNode->right == _node)
		parentOfNode->right = rightChild;
	else
		parentOfNode->left = rightChild;

	rightChild->parent = parentOfNode;
}

template<KEY KeyType, typename ValueType>
void RBTree<KeyType, ValueType>::rightRotate(Node* _node)
{

}

template<KEY KeyType, typename ValueType>
bool RBTree<KeyType, ValueType>::insert(const KeyType& _key, const ValueType& _value)
{
	if (!this->root)
	{
		this->root = new RBNode(_key, _value);
		this->root->setColor('B');
		++(this->m_size);
		return true;
	}

	Node* searchPtr = this->root;

	while (true)
	{
		if (_key == searchPtr->key)
			return false;

		if (_key > searchPtr->key)
		{
			if (!searchPtr->right)
			{
				searchPtr->right = new RBNode(_key, _value, searchPtr);
				++(this->m_size);
				this->last_added_node = searchPtr->right;
				break;
			}

			searchPtr = searchPtr->right;
		}
		else
		{
			if (!searchPtr->left)
			{
				searchPtr->left = new RBNode(_key, _value, searchPtr);
				++(this->m_size);
				this->last_added_node = searchPtr->left;
				break;
			}

			searchPtr = searchPtr->left;
		}
	}

	//≈сли родитель был черный, значит балансировка не нужна
	if (this->last_added_node->parent->getColor() == 'B')
		return true;

	insertBalance();
}

template<KEY KeyType, typename ValueType>
bool RBTree<KeyType, ValueType>::erase(const KeyType& _key)
{
	Node* nodeToErase = this->innerFind(_key);
	if (!nodeToErase)
		return false;
	
	// —оздаем структуру дл€ хранени€ информации дл€ балансировки после удалени€ элемента
	structForEraseBalance sfeb; 

	// ƒалее вызваем sfeb.setNode() дл€ узла который будет физически удален
	// и выполн€ем обычный алгоритм удалени€
	if (nodeToErase->left && nodeToErase->right)
	{
		Node* newNode = nodeToErase->left;
		while (newNode->right)
			newNode = newNode->right;

		sfeb.setNode(newNode);
		this->swapNodes(nodeToErase, newNode);
		this->parent_of_last_erased_node = newNode->parent;

		if (newNode->left)
		{
			newNode->left->parent = this->parent_of_last_erased_node;

			if (this->parent_of_last_erased_node->right == newNode)
				this->parent_of_last_erased_node->right = newNode->left;
			else
				this->parent_of_last_erased_node->left = newNode->left;

			delete newNode;
			--(this->m_size);
			eraseBalance(sfeb);
			return true;
		}


		if (this->parent_of_last_erased_node->right == newNode)
			this->parent_of_last_erased_node->right = nullptr;
		else
			this->parent_of_last_erased_node->left = nullptr;

		delete newNode;
		--(this->m_size);
		eraseBalance(sfeb);
		return true;
	}

	sfeb.setNode(nodeToErase); //nodeToErase будет фактически удален
	this->parent_of_last_erased_node = nodeToErase->parent;

	if (nodeToErase->left)
	{	
		//≈сли удал€емый элемент был корнем, достаточно просто перекрасить новый корень в черный
		if (nodeToErase == this->root)
		{
			this->root = this->root->left;
			this->root->parent = nullptr;

			delete nodeToErase;
			--(this->m_size);
			return true;
		}

		if (this->parent_of_last_erased_node->right == nodeToErase)
			this->parent_of_last_erased_node->right = nodeToErase->left;
		else
			this->parent_of_last_erased_node->left = nodeToErase->left;

		nodeToErase->left->parent = this->parent_of_last_erased_node;

		delete nodeToErase;
		--(this->m_size);
		eraseBalance(sfeb);

		return true;
	}

	if (nodeToErase->right)
	{
		//≈сли удал€емый элемент был корнем, достаточно просто перекрасить новый корень в черный
		if (nodeToErase == this->root)
		{
			this->root = this->root->right;
			this->root->parent = nullptr;

			delete nodeToErase;
			--(this->m_size);
			return true;
		}

		return true;
	}
}

//------------------------------------------------------------------------------------------------------
//-------------------------------------------- CLASS RBTREE --------------------------------------------
//------------------------------------------------ END -------------------------------------------------
#endif
