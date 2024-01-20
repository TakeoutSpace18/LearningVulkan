#ifndef NONMOVEABLE_H
#define NONMOVEABLE_H


class NonMoveable
{
protected:
    NonMoveable() = default;
    ~NonMoveable() = default;
private:
    NonMoveable(const NonMoveable&&);
    NonMoveable& operator=(const NonMoveable&&);
};

#endif //NONMOVEABLE_H
