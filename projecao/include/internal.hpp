#ifndef __LINALG_INTERNAL__
#define __LINALG_INTERNAL__

#include <functional>
#include <stdexcept>

namespace internal {
#ifdef NOEXCEPT
template <typename Pred>
static inline void validate(const char* message, Pred predicate) noexcept {}
#else
template <typename Pred>
static inline void validate(const char* message, Pred predicate) {
    if (!predicate()) {
        throw std::invalid_argument(message);
    }
}
#endif
} // namespace internal

#endif // __LINALG_INTERNAL__
