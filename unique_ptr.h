#ifndef UNIQUE_UNIQUE_PTR_H
#define UNIQUE_UNIQUE_PTR_H

#include <memory>

namespace {
    template <typename T>
    struct pointer
    {
        using type = T*;
    };
}

template <typename T>
class unique_ptr
{
    using PointerType = typename pointer<T>::type;

private:
    class DeleterStorageTypeBase
    {
    public:
        DeleterStorageTypeBase() {};
        DeleterStorageTypeBase(DeleterStorageTypeBase const&) = delete;
        DeleterStorageTypeBase& operator=(DeleterStorageTypeBase const&) = delete;
        virtual ~DeleterStorageTypeBase() {};
        virtual void invoke_deleter(PointerType ptr) = 0;
    };

    template <typename DeleterType>
    class DeleterStorageType : public DeleterStorageTypeBase
    {
        DeleterType deleter;

    public:
        DeleterStorageType(DeleterType deleter) : deleter(deleter) {};
        void invoke_deleter(PointerType ptr)
        {
            deleter(ptr);
        }
    };

    void destroy()
    {
        if (ptr && !deleter_storage)
            delete ptr;
        else if (ptr && deleter_storage)
        {
            deleter_storage->invoke_deleter(ptr);
            delete deleter_storage;
        } else if (deleter_storage)
            delete deleter_storage;
    }

public:
    unique_ptr() noexcept : ptr(nullptr), deleter_storage(nullptr) {};
    unique_ptr(unique_ptr const&) = delete;
    unique_ptr(unique_ptr&& other) noexcept :
        ptr(std::move(other.ptr)),
        deleter_storage(std::move(other.deleter_storage))
    {
        other.ptr = nullptr;
        other.deleter_storage = nullptr;
    }

    template <typename DeleterType>
    unique_ptr(PointerType ptr, DeleterType deleter = std::default_delete<T>()) :
        ptr(ptr),
        deleter_storage(new DeleterStorageType<DeleterType>(deleter))
    {}
    unique_ptr(PointerType ptr) noexcept :
        ptr(ptr),
        deleter_storage(nullptr)
    {}

    ~unique_ptr()
    {
        destroy();
    }

    unique_ptr& operator=(unique_ptr const&) = delete;

    unique_ptr& operator=(unique_ptr&& other) noexcept
    {
        destroy();
        ptr = std::move(other.ptr);
        deleter_storage = std::move(deleter_storage);
        other.ptr = nullptr;
        other.deleter_storage = nullptr;
    }

    PointerType release()
    {
        auto tmp = ptr;
        ptr = nullptr;
        destroy();
        return tmp;
    }

    template <typename DeleterType = std::default_delete<T>>
    void reset(PointerType ptr = nullptr, DeleterType deleter = DeleterType())
    {
        destroy();
        this->ptr = ptr;
        this->deleter_storage = new DeleterStorageType(deleter);
    }

    void swap(unique_ptr<T>& other) noexcept
    {
        std::swap(ptr, other.ptr);
        std::swap(deleter_storage, other.deleter_storage);
    }

    PointerType get() noexcept
    {
        return ptr;
    }

    explicit operator bool() noexcept
    {
        return get();
    }

    T& operator*() noexcept
    {
        return *get();
    }

    PointerType operator->() noexcept
    {
        return get();
    }

private:
    PointerType ptr;
    DeleterStorageTypeBase* deleter_storage;
};

template <typename T, typename ...Args>
unique_ptr<T> make_unique(Args&& ...args)
{
    return unique_ptr<T>(new T(std::forward<Args>(args)...));
}

#endif //UNIQUE_UNIQUE_PTR_H
