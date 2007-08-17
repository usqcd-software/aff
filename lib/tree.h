#ifndef MARK_c57377cb_879b_4716_a827_b5a4428d7fbc
#define MARK_c57377cb_879b_4716_a827_b5a4428d7fbc

/* Tree table implementation.
 *
 * foreach() passes elements of the table in order of ids, root is not returned.
 * insert() guarantees dense packing of indices, NULL is returned if there is a
 *          duplicate or not enough memory.
 * size() returns the largest valid Id in the tree;
 * file_size() returns the number of bytes needed to store the tree in a file.
 * root() returns an a root node of the tree.
 * lookup() returns NULL if the node does not exist.
 * n_*() are accessors to the TreeNode structure.
 */
struct AffTree_s;
struct AffTreeNode_s;

enum AffNodeType_e {
    affNodeInvalid,
    affNodeVoid,
    affNodeChar,
    affNodeInt,
    affNodeDouble,
    affNodeComplex
};

struct AffTree_s *aff_tt_init(void);
void *aff_tt_fini(struct AffTree_s *tt);
void aff_tt_foreach(const struct AffTree_s *tt,
                    void (*proc)(const struct AffTreeNode_s *, void *),
                    void *arg);
void aff_tt_print(const struct AffTree_s *tt);
struct AffTreeNode_s *aff_tt_root(const struct AffTree_s *tt);
struct AffTreeNode_s *aff_tt_lookup(const struct AffTree_s *tt,
				    const struct AffTreeNode_s *parent,
				    uint32_t nameId);
struct AffTreeNode_s *aff_tt_insert(struct AffTree_s *tt,
				    struct AffTreeNode_s *parent,
				    uint32_t nameId,
				    enum AffNodeType_e type,
				    uint32_t size,
				    uint64_t offset);
uint64_t aff_tt_size(const struct AffTree_s *tn);
uint64_t aff_tt_file_size(const struct AffTree_s *tn);
void aff_n_foreach(const struct AffTreeNode_s *tn,
		   void (*proc)(const struct AffTreeNode_s *, void *),
		   void *arg);
uint64_t aff_n_parent(const struct AffTreeNode_s *tn);
uint32_t aff_n_name(const struct AffTreeNode_s *tn);
uint64_t aff_n_id(const struct AffTreeNode_s *tn);
enum AffNodeType_e aff_n_type(const struct AffTreeNode_s *tn);
uint32_t aff_n_size(const struct AffTreeNode_s *tn);
uint64_t aff_n_offset(const struct AffTreeNode_s *tn);


#endif /* !defined(MARK_c57377cb_879b_4716_a827_b5a4428d7fbc) */
