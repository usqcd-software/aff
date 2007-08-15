#ifndef MARK_0f45bcf_7bd0_45f0_9e25_03e1b803e0bc
#define MARK_0f45bcf_7bd0_45f0_9e25_03e1b803e0bc
/* All functions returning int: 0 on success, non zero on errors */
/* Q: Do we need error codes and conversion of errors to strings? */

/* (?) Of is this a visible interface? */

/**** string table ****/
typedef struct AffSTable_s AffSTable;
typedef uint32_t AffStringId;
AffSTable *aff_st_init(void);
void aff_st_fini(AffSTable *stable);
AffSTable *aff_st_load(AFF *affile, uint64_t offset, uint64_t size);
int aff_st_store(AFF *affile, uint64_t offset, uint64_t size, AffSTable *st);
uint64_t aff_st_size(AffSTable *stable);
uint64_t aff_st_file_size(AffSTable *stable);
AffStringId aff_st_index(AffSTable *stable, const char *name);
const char *aff_st_value(AffSTable *stable, AffStringId index);
AffStringId aff_st_insert(AffSTable *stable, const char *name);
void aff_st_foreach(AffSTable *stable,
                    void (*proc)(const char *name, AffStringId idx, void *arg),
                    void *arg);

/**** Node Types */
typedef enum {
    AffEmptyNode = 0,
    AffIntNode = 1,
    AffCharNode = 2,
    AffDoubleNode = 3,
    AffComplexNode = 4,
    Aff_MAX_Node = 4
} AffNodeType;
/* XXX */

/**** Node IDs */
typedef uint64_t AffNodeId;
/* XXX
 *  access operations: get type, make NodeId of num and type, root and null 
 * Q: could there be data in the root node?
 */

/**** tree table ****/
typedef struct AffTTable_s AffTTable;

AffTTable *aff_tt_init(void);
void aff_ttable_fini(AffTTable *tt);
AffTTable *aff_tt_load(AFF *affile, uint64_t offset, uint64_t size);
int aff_tt_store(AFF *affile, uint64_t offset, uint64_t size, AFFTTable *tt);
uint64_t aff_tt_file_size(AffTTable *ttable);
AffNodeId aff_tt_lookup(AffTTable *tt, AffNodeId pId, AffStringId nId);
int aff_tt_insert(AffTable *tt, AffNodeId pId, AffStringId nId, AffNodeId cId);
void aff_tt_foreach(AffTable *tt,
                    void (*proc)(AffNodeId pId, AffStringId nId, AffNodeId cId,
                                 void *arg),
                    void *arg);


/**** data table ****/
typedef struct AffDTable_s AffDTable;
AffDTable *aff_dt_init(void);
void aff_dt_fini(AffDTable *dt);
AffDTable *aff_dt_load(AFF *affile, uint64_t offset, uint64_t soze);
int aff_dt_store(AFF *affile, uint64_t offset, uint64_t size, AffDTable *dt);
uint64_t aff_dt_file size(AffDTable *dtable);
/* (?)  */

#endif

