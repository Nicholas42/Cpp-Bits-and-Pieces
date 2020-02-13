A vector with a static size. It is also able to take ownership of raw pointers. 

On the design
======

There are mostly two use cases for this design 

 * As a replacement for `std::vector` if no dynamic size is needed.
 * As an improved wrapper around a raw array (compared to `std::unique_ptr<T[]>`).

This implied some important parts of the design
 
 * An interface very similar to `std::vector` should be provided
 * There should be the possibility to both acquire and release the ownership of a raw pointer
 
There is a significant difference between `std::vector` and a raw array allocated with `new T[]`. In the array, allocation and construction are done in one step. The `std::vector` also owns uninitialized memory and separates allocation and construction. This implies that a raw array only works for default constructible types, while an `std::vector` may also be used for other types.

I wanted to retain the ability to work with non-default constructible types, so I decided on templating the vector with an allocator. But I further think that the static vector should not own uninitialized memory but only an array of initialized types. Hence all constructors will generate an underlying array of completely initialized objects. 

This brought the problem what to do if a constructor throws. We have to destroy all already constructed elements and hence need to know which those are. Since I neither wanted to add a member variable that indicates how far the construction has come (since that would be useless after construction) nor make the size non-const (admittedly, this would be not much of a problem if we never expose it in a mutable way, but yay for const-correctness!), I decided to add a factory class `vector_factory`.

This class is private in the vector and never exposed. It will first allocate memory in a private constructor and then construct objects in this memory depending on the input, tracking what is the last element we managed to construct. The benefit of the private constructor is, that the public constructors can delegate to it, and hence there is a completed object on that the destructor will be called in case of an exception during the construction of the array elements. Schematic example:

 
 * Call to `vector(size_t size, const T& value, const allocator_type &alloc)` (initializes a vector with `size` copies of `value`, using `alloc` for allocations)
    * Calls `vector_factory(size_t size, allocator_type alloc, const T& value)`
        *  Calls `vector_factory(size_t size, allocator_type alloc)` 
            * This allocates `size * sizeof(T)` bytes with `alloc`. In case of exception, stack unwinding will not call any destructor of the objects in construction. The allocator is responsible for cleaning up the allocation that did not work.
        * Now we have a complete `vector_factory`
        * Constructing `size` copies of `value` in the allocated space, tracking a pointer past the last constructed element in member object `m_end`.
            * If this throws, stack unwinding will call `~vector_factory()`. The destructor will destroy all already constructed elements in reverse order, starting with `*(m_end - 1)`. After that the allocated space will be deallocated.
    * Calls `vector(vector&& vector_factory)` with the just constructed factory. This steals the data pointer of the factory and replaces it with a `nullptr`, so that the destructor of the factory does not delete the resource.

