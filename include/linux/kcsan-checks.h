/* SPDX-License-Identifier: GPL-2.0 */

#ifndef _LINUX_KCSAN_CHECKS_H
#define _LINUX_KCSAN_CHECKS_H

#include <linux/types.h>

/*
 * Access type modifiers.
 */
#define KCSAN_ACCESS_WRITE  0x1
#define KCSAN_ACCESS_ATOMIC 0x2

/*
 * __kcsan_*: Always calls into the runtime when KCSAN is enabled. This may be used
 * even in compilation units that selectively disable KCSAN, but must use KCSAN
 * to validate access to an address. Never use these in header files!
 */
#ifdef CONFIG_KCSAN
/**
 * __kcsan_check_access - check generic access for data races
 *
 * @ptr address of access
 * @size size of access
 * @type access type modifier
 */
void __kcsan_check_access(const volatile void *ptr, size_t size, int type);

/**
 * kcsan_nestable_atomic_begin - begin nestable atomic region
 *
 * Accesses within the atomic region may appear to race with other accesses but
 * should be considered atomic.
 */
void kcsan_nestable_atomic_begin(void);

/**
 * kcsan_nestable_atomic_end - end nestable atomic region
 */
void kcsan_nestable_atomic_end(void);

/**
 * kcsan_flat_atomic_begin - begin flat atomic region
 *
 * Accesses within the atomic region may appear to race with other accesses but
 * should be considered atomic.
 */
void kcsan_flat_atomic_begin(void);

/**
 * kcsan_flat_atomic_end - end flat atomic region
 */
void kcsan_flat_atomic_end(void);

/**
 * kcsan_atomic_next - consider following accesses as atomic
 *
 * Force treating the next n memory accesses for the current context as atomic
 * operations.
 *
 * @n number of following memory accesses to treat as atomic.
 */
void kcsan_atomic_next(int n);

#else /* CONFIG_KCSAN */

static inline void __kcsan_check_access(const volatile void *ptr, size_t size,
					int type) { }

static inline void kcsan_nestable_atomic_begin(void)	{ }
static inline void kcsan_nestable_atomic_end(void)	{ }
static inline void kcsan_flat_atomic_begin(void)	{ }
static inline void kcsan_flat_atomic_end(void)		{ }
static inline void kcsan_atomic_next(int n)		{ }

#endif /* CONFIG_KCSAN */

/*
 * kcsan_*: Only calls into the runtime when the particular compilation unit has
 * KCSAN instrumentation enabled. May be used in header files.
 */
#ifdef __SANITIZE_THREAD__
#define kcsan_check_access __kcsan_check_access
#else
static inline void kcsan_check_access(const volatile void *ptr, size_t size,
				      int type) { }
#endif

/**
 * __kcsan_check_read - check regular read access for data races
 *
 * @ptr address of access
 * @size size of access
 */
#define __kcsan_check_read(ptr, size) __kcsan_check_access(ptr, size, 0)

/**
 * __kcsan_check_write - check regular write access for data races
 *
 * @ptr address of access
 * @size size of access
 */
#define __kcsan_check_write(ptr, size)                                         \
	__kcsan_check_access(ptr, size, KCSAN_ACCESS_WRITE)

/**
 * kcsan_check_read - check regular read access for data races
 *
 * @ptr address of access
 * @size size of access
 */
#define kcsan_check_read(ptr, size) kcsan_check_access(ptr, size, 0)

/**
 * kcsan_check_write - check regular write access for data races
 *
 * @ptr address of access
 * @size size of access
 */
#define kcsan_check_write(ptr, size)                                           \
	kcsan_check_access(ptr, size, KCSAN_ACCESS_WRITE)

/*
 * Check for atomic accesses: if atomic accesses are not ignored, this simply
 * aliases to kcsan_check_access(), otherwise becomes a no-op.
 */
#ifdef CONFIG_KCSAN_IGNORE_ATOMICS
#define kcsan_check_atomic_read(...)	do { } while (0)
#define kcsan_check_atomic_write(...)	do { } while (0)
#else
#define kcsan_check_atomic_read(ptr, size)                                     \
	kcsan_check_access(ptr, size, KCSAN_ACCESS_ATOMIC)
#define kcsan_check_atomic_write(ptr, size)                                    \
	kcsan_check_access(ptr, size, KCSAN_ACCESS_ATOMIC | KCSAN_ACCESS_WRITE)
#endif

#endif /* _LINUX_KCSAN_CHECKS_H */
