#ifndef NONMOVABLE_H
#define NONMOVABLE_H


class NonMovable
{
protected:
    NonMovable() = default;
    ~NonMovable() = default;
private:
    NonMovable(const NonMovable&&);
    NonMovable& operator=(const NonMovable&&);
};

#endif //NONMOVABLE_H
