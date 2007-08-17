#ifndef MARK_c7329614_7d7a_4efe_9f6d_87477961bc99
#define MARK_c7329614_7d7a_4efe_9f6d_87477961bc99

/* AFF reverse path structure */
struct AffReversePath_s {
    const struct AffReversePath_s *parent;
    const char *name;
};

/* Aff nodes.
 *  These are used both in the reader and the writer.
 * cd*() walk the tree. If the create argument is zero, missing elements of
 *       the pass will result in error (NULL is returned), otherwise, missing
 *       components will be created in the path. Freshly created components
 *       could be assigned, their initial type is affNodeVoid.
 * foreach() traverses children of the node.
 * name(), parent(), type(), and size() access attribute of the node.
 */
struct AffNode_s;

struct AffNode_t *aff_node_cdp(struct AffNode_s *n,
			       int create,
			       struct AffReversePath_s *p);
struct AffNode_t *aff_node_cdv(struct AffNode_s *n,
			       int create,
			       va_list va);
struct AffNode_t *aff_node_cd(struct AffSTable_s *stable,
			      struct AffNode_s *n,
			      int create,
			      const char *name, ...);
void aff_node_foreach(struct AffNode_s *n,
		      void (*proc)(struct AffNode_s *child,
				   void *arg),
		      void *arg);
const char *aff_node_name(struct AffNode_s *n);
struct AffNode_t *aff_node_parent(struct AffNode_s *n);
enum AffNodeType_e aff_node_type(struct AffNode_s *n);
uint32_t aff_node_size(struct AffNode_s *n);

/* AFF objects. Writers.
 *
 * writer() prepares to write an aff file. If it exists the file is deleted
 *          before opening.
 * writer_close() commits the data to the disk and closes the file. It returns 0
 *          if no error occured, an a non-zero error status otherwise.
 * writer_errstr() returns the first error recorded in the object.
 * put_*() write data to the file and set node's type. A put could be called
 *          at most once on a node.
 */
struct AffWriter_s;

struct AffWriter_s *aff_writer(const char *file_name);
int aff_writer_close(struct AffWriter_s *aff);
const char *aff_writer_errstr(struct AffWriter_s *aff);
struct AffSTable_s *aff_stable(struct AffWriter_s *aff);
struct AffNode_t *aff_writer_root(struct AffWriter_s *aff);

int aff_node_put_char(struct AffWriter_s *aff,
		      struct AffNode_s *n,
		      const char *d,
		      uint32_t s);
int aff_node_put_int(struct AffWriter_s *aff,
		     struct AffNode_s *n,
		     const int32_t *d,
		     uint32_t s);
int aff_node_put_double(struct AffWriter_s *aff,
			struct AffNode_s *n,
			const double *d,
			uint32_t s);
int aff_node_put_complex(struct AffWriter_s *aff,
			 struct AffNode_s *n,
			 const double _Complex *d,
			 uint32_t s);

/* AFF objects. Readers.
 *
 * reader() loaded the index from the file.
 * close_reader() closes the file and frees all data associated with the reader.
 * reader_errstr() returns the first error recorded in the object.
 * foreach() traverses all nodes in the tree.
 * get_*() are accessors to the data, partial reads return the head, long reads
 *         ignore tail.
 */
struct AffReader_s;
struct AffReaderNode_s;

struct AffReader_s *aff_reader(const char *file_name);
int aff_close_reader(struct AffReader_s *aff);
const char *aff_reader_errstr(struct AffReader_s *aff);
struct AffSTable_s *aff_reader_stable(const struct AffReader_s *aff);
struct AffNode_t *aff_reader_root(struct AffReader_s *aff);
void aff_reader_foreach(struct AffReader_s *aff,
			void (*proc)(struct AffNode_s *node,
				     void *arg),
			void *arg);
int aff_node_get_char(const struct AffReader_s *aff,
		      const struct AffNode_s *n,
		      char *d,
		      uint32_t s);
int aff_node_get_int(const struct AffReader_s *aff,
		     const struct AffNode_s *n,
		     int32_t *d,
		     uint32_t s);
int aff_node_get_double(const struct AffReader_s *aff,
			const struct AffNode_s *n,
			double *d,
			uint32_t s);
int aff_node_get_complex(const struct AffReader_s *aff,
			 const struct AffNode_s *n,
			 double _Complex *d,
			 uint32_t s);


#endif /* !defined(MARK_c7329614_7d7a_4efe_9f6d_87477961bc99) */
