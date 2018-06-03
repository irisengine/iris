#pragma once

#include <iterator>
#include <memory>
#include <queue>
#include <utility>
#include <vector>

namespace eng
{

/**
 * An STL-esq container that stores elements in a k-ary tree. Each child has
 * a pointer to its parent but not other children, because of this it is
 * possible to move from any node to the root.
 *
 * When using iterators the current node is returned with begin() and end() is
 * always the root (i.e. one past the first element inserted), this means that
 * when iterating elements are visited in the reverse order that they were
 * inserted.
 *
 * Whilst every effort has been made to make the container as STL-like as
 * possible there are certain obvious omissions including: size(), empty(),
 * operator==, etc
 * These are not implemented as they don't make sense for this container type,
 * for example what should size() return? The total number of nodes, or the
 * number of nodes that will be visited when iterating (i.e. the path to root)?
 * As this information isn't really useful it is not provided.
 */
template<class T>
class cactus_stack
{
    // forward declaration
    struct cactus_stack_iterator;

    public:

        // member types
        using value_type = T;
        using reference = value_type&;
        using const_reference = const value_type&;
        using iterator = cactus_stack_iterator;
        using const_iterator = const cactus_stack_iterator;
        using difference_type = std::ptrdiff_t;
        using size_type = std::size_t;

        /**
         * Construct a new cactus_stack container.
         */
        cactus_stack()
            : head_node_(std::make_unique<node_base>()),
              cursor_(head_node_.get())
        { }

        // default
        ~cactus_stack() = default;
        cactus_stack(cactus_stack&&) noexcept = default;
        cactus_stack& operator=(cactus_stack&&) noexcept = default;

        /**
         * Copy constructor. Performs a deep copy of all inserted elements.
         *
         * @param other
         *   Other cactus_stack to copy.
         */
        cactus_stack(const cactus_stack &other)
            : head_node_(std::make_unique<node_base>()),
              cursor_(head_node_.get())
        {
            // create a queue of nodes to visit, starting with the supplied
            // other cactus_stack head
            std::queue<node_base*> nodes_other{ };
            nodes_other.emplace(other.head_node_.get());

            // create a queue of nodes inserted, starting with this head node
            // this allows us to walk the other tree and duplicate it
            std::queue<node_base*> nodes_this{ };
            nodes_this.emplace(head_node_.get());

            // breadth-first visit all nodes in the other cactus_stack, copying
            // each node into this
            while(!nodes_other.empty())
            {
                // get the next node to process from its queue
                const auto *parent_other = nodes_other.front();
                nodes_other.pop();

                // get the corresponding node from this cactus_stack from its
                // queue
                auto *parent_this = nodes_this.front();
                nodes_this.pop();

                // iterate through all the children of the other node
                for(const auto &child : parent_other->children)
                {
                    // add the children to the processing queue
                    nodes_other.emplace(child.get());

                    // copy the child node into our cactus_stack
                    const auto *child_node = static_cast<node*>(child.get());
                    parent_this->children.emplace_back(std::make_unique<node>(child_node->value_));
                    parent_this->children.back()->parent = parent_this;

                    // we want this cursor to point to the same equivalent node
                    // as the other cactus stack cursor
                    if(other.cursor_ == child.get())
                    {
                        cursor_ = parent_this->children.back().get();
                    }

                    // add the newly created child to our processing queue
                    nodes_this.emplace(parent_this->children.back().get());
                }
            }
        }

        /**
         * Copy assignment operator.
         *
         * @param other
         *   The other cactus_stack to copy from. Note that this is passed by
         *   value, this provides us a strong guarantee that if copying throws
         *   an exception then this object is left unchanged (as the actual
         *   function will never be called).
         *
         * @returns
         *   This cactus_stack after copying.
         */
        cactus_stack& operator=(cactus_stack other)
        {
            // simply swap and return with other
            swap(other);
            return *this;
        }

        /**
         * Swap this cactus_stack with another.
         *
         * @param other
         *   cactus_stack to swap with.
         */
        void swap(cactus_stack &other)
        {
            // simply swap private members
            std::swap(other.head_node_, head_node_);
            std::swap(other.cursor_, cursor_);
        }

        /**
         * Get a reference to the topmost element, which is the most recently
         * added element. Call top on an empty cactus_stack is undefined
         * behaviour.
         *
         * @returns
         *   Reference to last added element.
         */
        reference top()
        {
            return *begin();
        }

        /**
         * Get a const reference to the topmost element, which is the most
         * recently added element. Call top on an empty cactus_stack is
         * undefined behaviour.
         *
         * @returns
         *   Const reference to last added element.
         */
        const_reference top() const
        {
            return *cbegin();
        }

        /**
         * Add an element to the cactus_stack.
         *
         * This element will be added as a child to the current top() element,
         * then top() will become this element.
         *
         * @param value
         *   Value to add.
         */
        void push(const_reference value)
        {
            emplace(std::move(value));
        }

        /**
         * Add an element to the cactus_stack, using perfect forwarding.
         *
         * This element will be added as a child to the current top() element,
         * then top() will become this element.
         *
         * @param value
         *   Value to add.
         */
        template<class ...Args>
        void emplace(Args &&...args)
        {
            // create new node with supplied arguments
            auto new_node = std::make_unique<node>(std::forward<Args>(args)...);
            new_node->parent = cursor_;

            // insert new node as child of cursor, then point cursor to the
            // new node
            cursor_->children.emplace_back(std::move(new_node));
            cursor_ = cursor_->children.back().get();
        }

        /**
         * Set top() to be the parent of the current top().
         *
         * Note that this does not delete any element, as the whole point of
         * a cactus_stack is to hold all inserted elements but be able to
         * traverse upwards towards the root.
         *
         * Calling this on an empty cactus_stack is undefined behaviour.
         */
        void pop()
        {
            cursor_ = cursor_->parent;
        }

        /**
         * Remove all elements.
         */
        void clear()
        {
            head_node_ = std::make_unique<node>();
            cursor_ = head_node_.get();
        }

        /**
         * Get iterator to last element inserted.
         *
         * @returns
         *   Iterator to last inserted element.
         */
        iterator begin()
        {
            return iterator{ cursor_ };
        }

        /**
         * Get const iterator to last element inserted.
         *
         * @returns
         *   Const Iterator to last inserted element.
         */
        const_iterator begin() const
        {
            return cbegin();
        }

        /**
         * Get const iterator to last element inserted.
         *
         * @returns
         *   Const Iterator to last inserted element.
         */
        const_iterator cbegin() const
        {
            return const_iterator{ cursor_ };
        }

        /**
         * Get iterator to one past the end of the cactus_stack.
         *
         * @returns
         *   Iterator to one past end of cactus_stack.
         */
        iterator end()
        {
            return iterator{ head_node_.get() };
        }

        /**
         * Get const iterator to one past the end of the cactus_stack.
         *
         * @returns
         *   Const iterator to one past end of cactus_stack.
         */
        const_iterator end() const
        {
            return cend();
        }

        /**
         * Get const iterator to one past the end of the cactus_stack.
         *
         * @returns
         *   Const iterator to one past end of cactus_stack.
         */
        const_iterator cend() const
        {
            return const_iterator{ head_node_.get() };
        }

        /**
         * Set top() to be the element in the supplied iterator.
         *
         * @param iter
         *   Iterator to new top() element.
         */
        void advance(const cactus_stack_iterator &iter)
        {
            cursor_ = iter.node_;
        }

    private:

        /**
         * Struct which encapsulates the base of a node in the k-ary tree. This
         * simply contains pointers to other nodes and does not store a value.
         * This will form the root node of the cactus_stack and because it does
         * not store a value then an empty_cactus stack can be constructed
         * even if its value_type is not default constructable.
         */
        struct node_base
        {
            // default
            node_base() = default;
            virtual ~node_base() = default;
            node_base(const node_base&) = default;
            node_base& operator=(const node_base&) = default;
            node_base(node_base&&) = default;
            node_base& operator=(node_base&&) = default;

            /** Pointer to parent. */
            node_base *parent = nullptr;

            /** Collection of children nodes. */
            std::vector<std::unique_ptr<node_base>> children = { };
        };

        /**
         * Derived node_base struct which stores an actual value.
         */
        struct node final : public node_base
        {
            /**
             * Construct a new node.
             *
             * @param args
             *   Arguments to be perfectly forwarded to the value constructor.
             */
            template<class ...Args>
            node(Args &&...args)
                : node_base(),
                  value_(std::forward<Args>(args)...)
            { }

            // default
            ~node() override = default;
            node(const node&) = default;
            node& operator=(const node&) = default;
            node(node&&) = default;
            node& operator=(node&&) = default;

            /** Stored value */
            value_type value_;
        };

        struct cactus_stack_iterator
        {
            // member types
            using iterator_category = std::input_iterator_tag;
            using value_type = T;
            using difference_type = std::ptrdiff_t;
            using size_type = std::size_t;
            using pointer = T*;
            using reference = T&;

            /**
             * Construct a cactus_stack_iterator.
             *
             * @param node
             *   Node iterator references.
             */
            explicit cactus_stack_iterator(node_base *node)
                : node_(node)
            { }

            // default
            cactus_stack_iterator() = default;
            ~cactus_stack_iterator() = default;
            cactus_stack_iterator(const cactus_stack_iterator&) = default;
            cactus_stack_iterator& operator=(const cactus_stack_iterator&) = default;
            cactus_stack_iterator(cactus_stack_iterator&&) = default;
            cactus_stack_iterator& operator=(cactus_stack_iterator&&) = default;

            /**
             * Derefernce iterator.
             *
             * @returns
             *   Reference to value in iterator.
             */
            reference operator*() const
            {
                // safe to cast as iterators with values all store nodes
                return static_cast<node*>(node_)->value_;
            }

            /**
             * Pre increment iterator. Moves iterator to parent.
             *
             * @param
             *   This iterator after advancing.
             */
            cactus_stack_iterator& operator++()
            {
                node_ = node_->parent;
                return *this;
            }

            /**
             * Post increment iterator. Moves iterator to parent.
             *
             * @param
             *   This iterator before advancing.
             */
            cactus_stack_iterator operator++(int)
            {
                auto new_iter = *this;
                ++(*this);
                return new_iter;
            }

            /**
             * Equality operator.
             *
             * @param other
             *   Iterator to compare with.
             *
             * @returns
             *   True if this iterator is the same element as supplied one,
             *   false otherwise.
             */
            bool operator==(const cactus_stack_iterator &other) const
            {
                return node_ == other.node_;
            }

            /**
             * Inequality operator.
             *
             * @param other
             *   Iterator to compare with.
             *
             * @returns
             *   True if this iterator is not the same element as supplied one,
             *   false otherwise.
             */
            bool operator!=(const cactus_stack_iterator &other) const
            {
                return !(*this == other);
            }

            /**
             * Advance iterator by specified amount.
             *
             * @param n
             *   Number of elements to advance by.
             *
             * @returns
             *   Reference to this iterator after advancing.
             */
            cactus_stack_iterator& operator+=(const difference_type n)
            {
                std::advance(*this, n);
                return *this;
            }

            /**
             * Advance iterator by specified amount.
             *
             * @param n
             *   Number of elements to advance by.
             *
             * @returns
             *   Copy of this iterator after advancing.
             */
            cactus_stack_iterator operator+(const difference_type n) const
            {
                return cactus_stack_iterator(*this) += n;
            }

            /** Node reference by the iterator. */
            node_base *node_;
        };

        /** Head node */
        std::unique_ptr<node_base> head_node_;

        /** Cursor which points to top() */
        node_base* cursor_;
};

}

