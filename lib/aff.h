#ifndef MARK_c7329614_7d7a_4efe_9f6d_87477961bc99
#define MARK_c7329614_7d7a_4efe_9f6d_87477961bc99

/* ???? is this enough? */

/* AFF reverse path structure */
struct AffReversePath_s {
    struct AffReversePath_s *parent;
    const char *name;
};

/* AFF objects. Writers.
 *
 * writer() prepares to write an aff file. If it exists the file is deleted
 *          before opening.
 * writer_close() commits the data to the disk and closes the file. It returns 0
 *                if no error occured, an a non-zero error status otherwise.
 * writer_errstr() converts error code to an informative string.
 */
struct AffWriter_s;

struct AffWriter_s *aff_writer(const char *file_name);
int aff_writer_close(struct AffWriter_s *aff);
const char *aff_writer_errstr(struct AffWriter_s *aff, int code);
int aff_writer_addp_char(struct AffWriter_s *aff,
			 const char *data,
			 uint32_t size,
			 const struct AffReversePath_s *path);
int aff_writer_add_char(struct AffWriter_s *aff,
			const char *data,
			uint32_t size,
			const char *path, ...);
int aff_writer_addp_int(struct AffWriter_s *aff,
			const int32_t *data,
			uint32_t size,
			const struct AffReversePath_s *path);
int aff_writer_add_int(struct AffWriter_s *aff,
		       const int32_t *data,
		       uint32_t size,
		       const char *path, ...);
int aff_writer_addp_double(struct AffWriter_s *aff,
			   const double *data,
			   uint32_t size,
			   const struct AffReversePath_s *path);
int aff_writer_add_double(struct AffWriter_s *aff,
			  const double *data,
			  uint32_t size,
			  const char *path, ...);
int aff_writer_addp_complex(struct AffWriter_s *aff,
			    const double _Complex *data,
			    uint32_t size,
			    const struct AffReversePath_s *path);
int aff_writer_add_complex(struct AffWriter_s *aff,
			   const double _Complex *data,
			   uint32_t size,
			   const char *path, ...);

/* AFF objects. Readers.
 *
 * reader() loaded the index from the file.
 * close_reader() closes the file and frees all data associated with the reader.
 *
 *
 */
struct AffReader_s;
struct AffReaderNode_s;

struct AffReader_s *aff_reader(const char *file_name);
int aff_close_reader(struct AffReader_s *aff);
const char *aff_reader_errstr(struct AffReader_s *aff, int code);
int aff_reader_getp_char(struct AffReader_s *aff,
			 char *data,
			 uint32_t size,
			 const struct AffReversePath_s *path);
int aff_reader_get_char(struct AffReader_s *aff,
			char *data,
			uint32_t size,
			const char *path, ...);
int aff_reader_getp_int(struct AffReader_s *aff,
			int32_t *data,
			uint32_t size,
			const struct AffReversePath_s *path);
int aff_reader_get_int(struct AffReader_s *aff,
		       int32_t *data,
		       uint32_t size,
		       const char *path, ...);
int aff_reader_getp_double(struct AffReader_s *aff,
			   double *data,
			   uint32_t size,
			   const struct AffReversePath_s *path);
int aff_reader_get_double(struct AffReader_s *aff,
			  double *data,
			  uint32_t size,
			  const char *path, ...);
int aff_reader_getp_complex(struct AffReader_s *aff,
			    double _Complex *data,
			    uint32_t size,
			    const struct AffReversePath_s *path);
int aff_reader_get_complex(struct AffReader_s *aff,
			   double _Complex *data,
			   uint32_t size,
			   const char *path, ...);
enum AffNodeType_e aff_reader_getp_type(struct AffReader_s *aff,
					const struct AffReversePath_s *path);
enum AffNodeType_e aff_reader_get_type(struct AffReader_s *aff,
				       const char *path, ...);
uint32_t aff_reader_getp_size(struct AffReader_s *aff,
			      const struct AffReversePath_s *path);
uint32_t aff_reader_get_size(struct AffReader_s *aff,
			     const char *path, ...);
void aff_reader_foreach(const struct AffReader_s *aff,
			void (*proc)(const struct AffReaderNode_s *, void *),
			void *arg);
const struct AffReaderNode_s *aff_reader_root(const AffReader_s *aff);
void aff_node_foreach(const struct AffReaderNode_s *node,
		      void (*proc)(const struct AffReaderNode_s *, void *),
		      void *arg);
enum AffNodeType_e aff_node_type(const struct AffReaderNode_s *n);
const char *aff_node_name(const struct AffReaderNode_s *n);
uint32_t aff_node_size(const struct AffReaderNode_s *n);
int aff_node_char(const struct AffReaderNode_s *n, char *d, uint32_t s);
int aff_node_int(const struct AffReaderNode_s *n, int32_t *d, uint32_t s);
int aff_node_double(const struct AffReaderNode_s *n, double *d, uint32_t s);
int aff_node_complex(const struct AffReaderNode_s *n,
		     double _Complex *d, uint32_t s);

#endif /* !defined(MARK_c7329614_7d7a_4efe_9f6d_87477961bc99) */
