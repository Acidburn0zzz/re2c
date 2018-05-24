#ifndef _RE2C_DFA_TAGTREE_
#define _RE2C_DFA_TAGTREE_

#include <stddef.h>
#include "src/util/c99_stdint.h"
#include <sys/types.h>
#include <vector>

#include "src/re/tag.h"
#include "src/util/forbid_copy.h"

namespace re2c
{

typedef uint32_t hidx_t;

static const hidx_t HROOT = ~0u;

struct clos_t;
typedef int bmatrix_t;

struct tagtree_t
{
	// the whole tree of tags found by the epsilon-closure
	// (a bunch of separate subtrees for each tag with common root)
	struct node_t {
		hidx_t pred;
		tagver_t elem;
		size_t tag;
	};
	std::vector<node_t> nodes;

	// reconstruct paths for comparison
	std::vector<ssize_t> path1;
	std::vector<ssize_t> path2;

	tagtree_t();
	hidx_t pred(hidx_t i) const;
	tagver_t elem(hidx_t i) const;
	size_t tag(hidx_t i) const;
	hidx_t push(hidx_t i, size_t t, tagver_t v);
	tagver_t last(hidx_t i, size_t t) const;
	int32_t compare_reversed(hidx_t x, hidx_t y, size_t t) const;
	int32_t precedence(const clos_t &x, const clos_t &y, int &rhox, int &rhoy,
		const bmatrix_t *bmatrix, const std::vector<Tag> &tags, size_t nclos);

	FORBID_COPY(tagtree_t);
};

} // namespace re2c

#endif // _RE2C_DFA_TAGTREE_
