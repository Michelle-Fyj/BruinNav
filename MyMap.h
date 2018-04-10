// MyMap.h

// Skeleton for the MyMap class template.  You must implement the first six
// member functions.
#ifndef MYMAP_INCLUDED
#define MYMAP_INCLUDED


template<typename KeyType, typename ValueType>
class MyMap
{
public:
    MyMap():m_size(0), m_root(nullptr){}
    ~MyMap(){FreeTree(m_root);}
	void clear();
    int size() const{return m_size;}
	void associate(const KeyType& key, const ValueType& value)
    {
        
        if (m_root == nullptr)
        {
            m_root = new Node(key, value);
            m_size++;
            return;
        }
        Node *current = m_root;
        for (;;)
        {
            if(key == current->m_key)
            {
                current->m_value = value;
                return;
            }
            if(key < current->m_key)
            {
                if(current->left != nullptr)
                    current = current->left;
                else
                {
                    current->left = new Node(key, value);
                    m_size++;
                    return;
                }
            }
            else if(key > current->m_key)
            {
                if(current->right != nullptr)
                    current = current->right;
                else
                {
                    current->right = new Node(key, value);
                    m_size++;
                    return;
                }
            }
        }
    }

	  // for a map that can't be modified, return a pointer to const ValueType
	const ValueType* find(const KeyType& key) const
    {
        if (m_root == nullptr) return nullptr;
        Node *cur = m_root;
        while(cur != nullptr)
        {
            if (key == cur->m_key)
                return &(cur->m_value);
            else if (key < cur->m_key)
                cur = cur->left;
            else
                cur = cur->right;
        }
        return nullptr;
    }

	  // for a modifiable map, return a pointer to modifiable ValueType
	ValueType* find(const KeyType& key)
	{
		return const_cast<ValueType*>(const_cast<const MyMap*>(this)->find(key));
	}

	  // C++11 syntax for preventing copying and assignment
	MyMap(const MyMap&) = delete;
	MyMap& operator=(const MyMap&) = delete;

private:
    struct Node
    {
    public:
        Node(KeyType key, ValueType value):m_key(key),m_value(value), left(nullptr), right(nullptr){}
        
        KeyType m_key;
        ValueType m_value;
        Node *left;
        Node *right;
    };

    Node *m_root;
    int m_size;
    void FreeTree(Node *cur)
    {
        if(cur == nullptr) return;
        FreeTree(cur->left);
        FreeTree(cur->right);
        delete cur;
    }
};


#endif // MYMAP_INCLUDED

//This class template will hold associations between an arbitrary type of key (e.g., a string containing an attraction name like “Barney’s Beanery”) with an arbitrary type of value (e.g., a latitude/longitude where that attraction is located).
