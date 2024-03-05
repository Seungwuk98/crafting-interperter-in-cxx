#ifndef __TYPE_UTILS__
#define __TYPE_UTILS__

#include <memory>
#include <optional>

template <typename U>
using uptr = std::unique_ptr<U>;

template <typename U, typename... Ts>
__always_inline uptr<U> mkuptr(Ts &&...args) {
  return std::make_unique<U>(std::forward<Ts>(args)...);
}

template <typename U>
using sptr = std::shared_ptr<U>;

template <typename U, typename... Ts>
__always_inline sptr<U> mksptr(Ts &&...args) {
  return std::make_shared<U>(std::forward<Ts>(args)...);
}

template <typename U>
using opt = std::optional<U>;

#endif
