#include <functional>

template<typename R, typename... Args>
class Delegate
{
public:
    Delegate(R f(Args...)): func_(f) {}

    R operator()(Args&&... args)
    {
        return func_(std::forward<Args>(args)...);
    }

private:
    std::function<R(Args...)> func_;
};

template<typename R, typename... Args>
Delegate<R, Args...> GetDelegate(R f(Args...))
{
    return Delegate<R, Args...>(f);
}
