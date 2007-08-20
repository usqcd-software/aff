#ifndef MARK_c7329614_7d7a_4efe_9f6d_87477961bc99
#define MARK_c7329614_7d7a_4efe_9f6d_87477961bc99

/* AFF imported types */
struct AffNode_s;

/* AFF objects. Writers.
 *
 * writer() prepares to write an aff file. If it exists the file is deleted
 *          before opening.
 * close() commits the data to the disk and closes the file. It returns 0
 *          if no error occured, an a non-zero error status otherwise.
 * errstr() returns the first error recorded in the object.
 * put_*() write data to the file and set node's type. A put could be called
 *          at most once on a node.
 * close() returns the last status of the writer. NULL means all the data was
 *          written to the file without errors, otherwise an error destription
 *          is returned. The error string is static and should *NOT* be freed().
 */

struct AffWriter_s;

struct AffWriter_s *aff_writer(const char *file_name);
const char *aff_writer_close(struct AffWriter_s *aff);
const char *aff_writer_errstr(struct AffWriter_s *aff);
struct AffSTable_s *aff_writer_stable(struct AffWriter_s *aff);
struct AffTree_s *aff_writer_tree(struct AffWriter_s *aff);
struct AffNode_s *aff_writer_root(struct AffWriter_s *aff);
struct AffNode_s *aff_writer_mkdir(struct AffWriter_s *aff,
				   struct AffNode_s *dir,
				   const char *name);
int aff_node_put_char(struct AffWriter_s *aff,
		      struct AffNode_s *n,
		      const char *d,
		      uint32_t s);
int aff_node_put_int(struct AffWriter_s *aff,
		     struct AffNode_s *n,
		     const uint32_t *d,
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
 * reader() loaded the indices from the file and keeps the file open for data
 *          access.
 * close() closes the file and frees all data associated with the reader.
 * check() computes the check sum on the data and returns 0 if everthing is
 *         is fine. Otherwise, an error message is set in the reader.
 * errstr() returns the first error recorded in the object.
 * get_*() are accessors to the data, partial reads return the head, long reads
 *         ignore tail.
 */
struct AffReader_s;

struct AffReader_s *aff_reader(const char *file_name);
void aff_reader_close(struct AffReader_s *aff);
const char *aff_reader_errstr(struct AffReader_s *aff);
struct AffTree_s *aff_reader_tree(struct AffReader_s *aff);
struct AffSTable_s *aff_reader_stable(const struct AffReader_s *aff);
int aff_reader_check(struct AffReader_s *aff);
struct AffNode_s *aff_reader_root(struct AffReader_s *aff);
struct AffNode_s *aff_reader_chdir(struct AffReader_s *aff,
				   struct AffNode_s *dir,
				   const char *name);
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
