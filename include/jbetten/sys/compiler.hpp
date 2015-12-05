//
// Created by jbetten on 12/5/15.
//

#ifndef PROJECT_COMPILER_HPP
#define PROJECT_COMPILER_HPP

namespace jbetten {

    static inline bool unlikely(bool x) {
        return __builtin_expect(x, 0);
    }

    static inline bool likely(bool x) {
        return __builtin_expect(x, 1);
    }

    static inline void prefetch(void *p) {
        __builtin_prefetch(p, 0, 3);
    }

    static inline void writehint(void *p) {
        __builtin_prefetch(p, 1, 3);
    }

};

#endif //PROJECT_COMPILER_HPP
