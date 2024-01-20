#ifndef NONCOPYABLE_H
#define NONCOPYABLE_H


class NonCopyable
{
protected:
    NonCopyable() = default;
    ~NonCopyable() = default;
private:
    NonCopyable(const NonCopyable&);
    NonCopyable& operator=(const NonCopyable&);
};

#endif //NONCOPYABLE_H
