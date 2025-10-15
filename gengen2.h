#ifndef GENGEN_H
#define GENGEN_H
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <time.h>
#include <stdbool.h>
#include <ctype.h>
#include <assert.h>
#ifdef _WIN32
#include <windows.h>
#define PATH_MAX MAX_PATH
#endif
#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

typedef struct {
	union {
		struct {
			const char *symbol;
		} normal;
		struct {
			const char* fmt;
		} formatted;
	};
	int type; // 0 = undef, 1 = normal, 2 = formatted
	const char *as;
} forward_item;
#define fwd(...) (forward_item) {__VA_ARGS__}
#define symbollit(sym) .normal.symbol=sym, .type = 1
#define symbolfmt(fmt_, ...) .formatted.fmt=fmt_,\
														 .type = 2

typedef struct {
	forward_item *fwd_items;
	size_t fwd_items_count, fwd_items_capacity;
} forward_table;

typedef struct {
	const char *needle, *with;
	int type; // 0 - stack allocated 'with', 1 - heap allocated 'with'
	int with_refcounter;
} replacement_item;

typedef struct {
	replacement_item *replacements;
	size_t replacements_count, replacements_capacity;
} replacement;

typedef struct {
	size_t count;
	const char* data;
} string_view;

#define assert_(cond, teardown)\
	if (!(cond)) {\
		teardown\
		exit(1);\
	}

typedef struct {
	const char *infilename, *outfilename_fmt;
} template_file;

struct dependency;
typedef struct ctemplate {
	const char* template_name;

	template_file* template_files;
	size_t template_files_count, template_files_cap;

	replacement replacement;

	struct dependency* deps;
	size_t deps_count, deps_cap;
} ctemplate;

typedef struct dependency_settings {
	bool embed;
	int embedloc;
} dependency_settings;
#define dep_settings(...) (dependency_settings) {__VA_ARGS__}

typedef struct dependency {
	forward_table fwd_table;
	ctemplate template_;
	dependency_settings settings;
} dependency;

typedef const char* path;
typedef struct generator_settings {
	path *search_paths;
	size_t path_count;
	const char *source_outdir, *header_outdir;
	bool verbose;
	bool dryrun;
	bool embed_deps; // ?
} generator_settings;
#define gen_settings_def() (generator_settings) {.search_path={"."}}
#define gen_settings(...) (generator_settings) {__VA_ARGS__}
#define paths(...) (path[]){__VA_ARGS__}, .path_count = (sizeof((path[]){__VA_ARGS__})/sizeof(path))

#define verbose_fprintf(fd, fmt, ...)\
	if (settings.verbose) fprintf(fd, __FILE__ ":%d  " fmt, __LINE__, __VA_ARGS__)

/*
 * @desc 			  Creates and initializes a ctemplate for use later
 * @notes 			Must be cleaned up with template_free
 * @return 			The created template instance
 */
ctemplate template_create(const char*);

/*
 * @desc 			  Free the internal memory used by the template
 * @param tplt  Pointer to the template being freed
 */
void template_free(ctemplate*);

/*
 * @desc 			         Adds a template file to the given template, and describes the output fileformat
 * @param tplt         Pointer to the template being modified
 * @param templatepath Path to the template file in your project
 * @param templatefmt  The format of the output file's name
 */
void template_addfile(ctemplate*, const char*, const char*);

/*
 * @desc 						Adds a new replacement instance to the template
 * @param tplt 			Pointer to the template being modified
 * @param symbol 		The symbol that should be replaced
 * @param with 			What should the symbol replaced with?
 */
void template_addreplacement(ctemplate*, const char*, const char*);

/* 
 * @desc 			 	     Adds a dependent template to this template
 * @param tplt 	     Pointer to the template being modified
 * @param dep_tplt   The template that this one depends on
 */
void template_adddep(ctemplate*, ctemplate, forward_table, dependency_settings);

/*
 * @desc 			  Creates and initializes a replacement for use later
 * @return 			The created replacement instance
 * @notes 			Must be cleaned up with replacement_free
 */
replacement replacement_create();

/*
 * @desc 				Frees the supplied replacement instance
 * @notes 			'repl' should have been created via replacement_create
 */
void replacement_free(replacement*);

/*
 * @desc 				Prints out this replacement
 * @param repl  Pointer to a replacement context
 * @notes 			'repl' should have been created via replacement_create
 */
void replacement_print(const replacement* repl);

/*
 * @desc 				Forwards the replacement from one replacement instance to another
 * 								while maintaining the original replacements
 * @return 			A new replacement instance 
 * @notes 			Returned instance must be freed with replacement_free
 */
replacement replacement_forward(generator_settings settings, replacement tplt, replacement dep_tplt, forward_table with); 

/*
 * @desc 					Adds a replacement item to this replacement context
 * @param repl 		Pointer to a replacement context
 * @param needle  The string to search for
 * @param with    The string to replace needle with
 * @notes 			  'repl' should have been created via replacement_create
 */
void replacement_add(replacement* repl, const char* needle, const char* with);

/*
 * @desc          Looks for the replacement for a given cursor position
 * @param repl    The replacement context to search in
 * @param cursor  The cursor position to search from
 * @return 		 		NULL if no replacement exists for the cursor position
 * 								pointer to replacement item that is used for the cursor position
 * @notes 				'repl' should have been created via 'replacement_create'
 */
replacement_item* replacement_get(replacement* repl, const char* cursor);

/*
 * @desc          Prints out the replacement data to stdout
 * @return 				A new forward table
 * @notes 				Returned table must be freed via 'forward_table_free'
 */
forward_table forward_table_create();

/*
 * @desc        		 Prints out the replacement data to stdout
 * @param fwd_table  A valid forward table pointer 
 * @notes 			     'fwd_table' should have been created via 'forward_table_create'
 */
void forward_table_free(forward_table* fwd_table);

/*
 * @desc        		 Adds a new forward rule to the forward table
 * @param fwd_table  A valid forward table pointer 
 * @param fwd_item   A valid forward item ('fwd' macro can be used for convience)
 * @notes 			     'fwd_table' should have been created via 'forward_table_create'
 */
void forward_table_forward(forward_table* fwd_table, forward_item fwd_item);

/*
 * @desc             Produces the final template given replacements and settings
 * @param settings   Provides a set of configuration options that influence how 
 *                      the generator functions
 *
 *                   .search_paths   - specify where the generator should look for templates
 *                   .outdir      - specify where the generator should put output templates
 *
 *                   @see settings_custom(...)
 *                   @see settings_default()
 * @param tplt       Specify what template we are generating
 * @param repl       Specify what replacement set we should use for generating
 * @notes            'tplt' should have been created via 'template_create'
 * @notes            'repl' should have been created via 'replacement_create'
 */
void generator_run(generator_settings settings, ctemplate tplt, replacement repl);

#endif

#define GENGEN_IMPLEMENTATION
#ifdef GENGEN_IMPLEMENTATION

typedef struct long_string_node {
	const char* content;
	size_t length;

	struct long_string_node *next, *prev;
} long_string_node;

typedef struct long_string_ll {
	long_string_node *head, *tail;
} long_string_ll;

ctemplate template_create(const char* name) {
  return (ctemplate) {
		.template_name = name,
		.template_files = NULL,
		.template_files_count = 0,
		.template_files_cap = 10,
		.deps_count = 0,
		.deps_cap = 10,
		.replacement = replacement_create()};
}

void template_free(ctemplate* tplt) {
	free(tplt->template_files);
	free(tplt->deps);
	replacement_free(&tplt->replacement);
}

void template_addfile(ctemplate* tplt, const char* templatepath, const char* templatefmt) {
	template_file file = (template_file){templatepath, templatefmt};
	if (tplt->template_files_count == 0) {
		tplt->template_files = (template_file*)calloc(tplt->template_files_cap, sizeof(ctemplate));
	}
	else if (tplt->template_files_count + 1 >= tplt->template_files_cap) {
		tplt->template_files = (template_file*)realloc(tplt->template_files, tplt->template_files_cap * 2);
		tplt->template_files_cap *= 2;
	}
	tplt->template_files[tplt->template_files_count++] = file;
}

void template_addreplacement(ctemplate *tplt, const char *symbol, const char *with) {
	replacement_add(&tplt->replacement, symbol, with);
}

void template_adddep(ctemplate* tplt, ctemplate dep_tplt, forward_table fwd_table, dependency_settings dep_settings) {
	if (tplt->deps_count == 0) {
		tplt->deps = (dependency*)calloc(tplt->deps_cap, sizeof(dependency));
	}
	if (tplt->deps_count + 1 >= tplt->deps_cap) {
		tplt->deps = (dependency*)realloc(tplt->deps, tplt->deps_cap * 2 * sizeof(dependency));
		tplt->deps_cap *= 2;
	}
	tplt->deps[tplt->deps_count++] = (dependency){.fwd_table = fwd_table, .template_ = dep_tplt, .settings = dep_settings};
}

replacement replacement_create() {
	return (replacement){.replacements = NULL, .replacements_count = 0, .replacements_capacity = 10};
}

void replacement_free(replacement* repl) {
	for (int i = 0; i < repl->replacements_count; i++) {
		replacement_item* item = &repl->replacements[i];
		if (item->type == 1) {
			item->with_refcounter--;
			if (item->with_refcounter == 0)
				free((char*)item->with);
		}
	}
	free(repl->replacements);
}

/*
 * forward replacements from 'from' to 'to' based on the 'with' forward table
 * Example:
 * from (replacement):
 * 	'$T' -> 'int'
 *  '^T' -> 'age'
 * to (replacment):
 * 	'@T' -> NULL
 * 	'&T' -> NULL
 * 	'PRINT' -> 'printf'
 * 	'FREE'  -> 'free'
 * with:
 * 	.symbol='$T', .as='@T'
 * 	.symbol='^T', .as='&T'
 *
 * expected:
 *  '@T' -> 'int'
 *  '^T' -> 'age'
 */
replacement replacement_forward(generator_settings settings, replacement to, replacement from, forward_table with) {
	// assert(0 && "Implement new fowarding algorithm that supports the ");
	replacement r = replacement_create();
	for (int i = 0; i < with.fwd_items_count; i++) {
		forward_item fwd = with.fwd_items[i];
		verbose_fprintf(stdout, "\033[33mForwarding: type=%d\n", fwd.type);

		// If the forward item is 'normal' (1) perform the normal forwarding logic
		if (fwd.type == 1) {
			int j = 0;
			// 1. find replacement in 'to' whose needle is equal to fwd.as
			for (j = 0; j < to.replacements_count; j++) {
				if (strncmp(to.replacements[j].needle, fwd.as, strlen(fwd.as)) == 0) {
					break;
				}
			}

			// if j is bigger than the replacement count we didnt find one
			//    j represents the index of the replacement in 'to' we are forwarding to
			if (j >= to.replacements_count) continue;

			int k = 0;
			// 2. find replacement in 'from' whose needle is equal to fwd.symbol
			for (k = 0; k < from.replacements_count; k++) {
				if (from.replacements[k].needle == fwd.normal.symbol) {
					break;
				}
			}

			// if k is bigger than the replacement count we didnt find one
			//    k represents the index of the replacement in 'from' we are forwarding from
			if (k >= from.replacements_count) continue;

			// at this point 'j' and 'k' should have valid indices
			// we need to set the .needle of to.replacments[j] equal to the .with of from.replacements[k]
			replacement_add(&r, to.replacements[j].needle, from.replacements[k].with);
			r.replacements[r.replacements_count - 1].type = 0;
		}
		// If the forward item is 'formatted' (2) perform formatted forwarding
		else if (fwd.type == 2) {
			// Setup the format string
			const char* fmtString = fwd.formatted.fmt;

			// Calculate how long the final formatted string will be
			const char* cursor = fmtString;
			char* formattedString = (char*)calloc(strlen(fmtString), sizeof(char));
			size_t formattedStringLen = 0;
			size_t formattedStringCap = strlen(fmtString);
			
			// Create the final formatted string
			// int state = 0;
			const char* beginfmtspec = NULL;
			const char* endfmtspec = NULL;
			while (*cursor) {
				if (*cursor == '{') {
					beginfmtspec = cursor + 1;
					while (*cursor && *cursor != '}') cursor++;
					endfmtspec = cursor;

					int k = 0;
					// find replacement for this fmt specifier
					for (k = 0; k < from.replacements_count; k++) {
						if (strncmp(from.replacements[k].needle, beginfmtspec, endfmtspec - beginfmtspec) == 0) {
							if (formattedStringLen + strlen(from.replacements[k].with) >= formattedStringCap) {
								formattedString = (char*)realloc(formattedString, formattedStringCap * 2);
								assert_(formattedString, {
									free(formattedString);
									formattedString = 0;
								});
								formattedStringCap *= 2;
							}
							strncat(formattedString, from.replacements[k].with, formattedStringCap);
							formattedStringLen += strlen(from.replacements[k].with);

							break;
						}
					}
					cursor++;
					continue;
				}
				formattedString[formattedStringLen++] = *cursor;
				cursor++;
			}
			formattedString[formattedStringLen] = 0;

			// Find where this argument gets forwarded
			for (int j = 0; j < to.replacements_count; j++) {
				if (strncmp(to.replacements[j].needle, fwd.as, strlen(fwd.as)) == 0) {
					replacement_add(&r, to.replacements[j].needle, formattedString);
					r.replacements[r.replacements_count - 1].type = 1;
					break;
				}
			}
			verbose_fprintf(stdout, "\033[33mFormatted: %s\n", formattedString);
		}
	}

	// when we get here, 'r' should contain all of the symbols from the forward table
	// now we need add back in the symbols that already had valid replacements
	for (int i = 0; i < to.replacements_count; i++) {
		// 1. determine if this replacement is NULL. if not we simply add it into the new repalcement
		if (to.replacements[i].with != NULL) {
			switch (to.replacements[i].type) {
				case 0: replacement_add(&r, to.replacements[i].needle, to.replacements[i].with);
								break;
				case 1: replacement_add(&r, to.replacements[i].needle, strdup(to.replacements[i].with));
								r.replacements[r.replacements_count - 1].with_refcounter++;
								break;
				default: assert(0 && "Unsupported replacement type");
			}
			r.replacements[r.replacements_count - 1].type = to.replacements[i].type;
		}
	}

	return r;
}
void replacement_add(replacement* repl, const char* needle, const char* with) {
	if (repl->replacements_count == 0) {
		repl->replacements = (replacement_item*)calloc(repl->replacements_capacity, sizeof(replacement));
	}
	if (repl->replacements_count + 1 >= repl->replacements_capacity) {
		repl->replacements = (replacement_item*)realloc(repl->replacements, sizeof(replacement) * repl->replacements_capacity * 2);
		repl->replacements_capacity *= 2;
	}
	replacement_item rep = {.needle = needle, .with = with, .with_refcounter = 0}; // static allocation of with
	repl->replacements[repl->replacements_count++] = rep;
}

replacement_item* replacement_get(replacement* repl, const char* cursor) {
	for (int i = 0; i < repl->replacements_count; i++) {
		replacement_item *item = &repl->replacements[i];
		size_t len = strlen(item->needle);
		if (strncmp(cursor, item->needle, len) == 0)
			return item;
	}
	return NULL;
}

/*
 * Time Complexity
 * O(n) in all cases
 * @desc          Prints out the replacement data to stdout
 * @param repl    The replacement context to search in
 * @notes 				'repl' should have been created via 'replacement_create'
 */
void replacement_print(const replacement* repl) {
	printf("Replacement {\n");
	for (int i = 0; i < repl->replacements_count; i++) {
		printf("\t{needle: %s, with: %s, %p}\n", repl->replacements[i].needle, repl->replacements[i].with, repl->replacements[i].with);
	}
	printf("}\n");
}

forward_table forward_table_create() {
	return (forward_table){.fwd_items = NULL, .fwd_items_count = 0, .fwd_items_capacity = 10};
}

void forward_table_free(forward_table* fwd_table) {
	free(fwd_table->fwd_items);
}

void forward_table_forward(forward_table* fwd_table, forward_item fwd_item) {
	if (fwd_table->fwd_items_count == 0) {
		fwd_table->fwd_items = (forward_item*)calloc(fwd_table->fwd_items_capacity, sizeof(forward_item));
	}
	if (fwd_table->fwd_items_count + 1 >= fwd_table->fwd_items_capacity) {
		fwd_table->fwd_items = (forward_item*)realloc(fwd_table->fwd_items, fwd_table->fwd_items_capacity * 2);
		fwd_table->fwd_items_capacity *= 2;
	}
	fwd_table->fwd_items[fwd_table->fwd_items_count++] = fwd_item;
}

/* Private function */
static char* read_file(const char* filepath) {
    FILE* f = fopen(filepath, "rb"); // 'rb' = binary read
    if (!f) {
        fprintf(stderr, "Failed to open '%s': %s\n", filepath, strerror(errno));
        return NULL;
    }

    // Move to end to get file size
    if (fseek(f, 0, SEEK_END) != 0) {
        fprintf(stderr, "fseek failed: %s\n", strerror(errno));
        fclose(f);
        return NULL;
    }

    long size = ftell(f);
    if (size < 0) {
        fprintf(stderr, "ftell failed: %s\n", strerror(errno));
        fclose(f);
        return NULL;
    }
    rewind(f); // Go back to beginning

    // Allocate buffer
    char* buffer = (char*)malloc((size_t)size + 1); // +1 for null terminator
    if (!buffer) {
        fprintf(stderr, "malloc failed\n");
        fclose(f);
        return NULL;
    }

    // Read file content
    size_t read = fread(buffer, 1, (size_t)size, f);
    if (read != (size_t)size) {
        if (ferror(f)) {
            fprintf(stderr, "fread failed: %s\n", strerror(errno));
        } else if (feof(f)) {
            fprintf(stderr, "Unexpected EOF while reading '%s'\n", filepath);
        } else {
            fprintf(stderr, "Partial read for '%s'\n", filepath);
        }
        free(buffer);
        fclose(f);
        return NULL;
    }

    buffer[size] = '\0'; // Null terminate
    fclose(f);
    return buffer;
}

// static char* read_file(const char* filepath) {
// 	FILE* f = fopen(filepath, "r");
// 	assert_(f, {
// 		fprintf(stderr, "Failed to open '%s'\n", filepath);
// 	});
// 	fseek(f, 0, SEEK_END);
// 	long size = ftell(f);
// 	assert_(size != -1, {
// 		fprintf(stderr, "Failed to ftell file '%s'. Reason: %s\n", filepath, strerror(errno));
// 	})
// 	fseek(f, 0, SEEK_SET);
// 
// 	char* buf = (char*)malloc((size_t)size + 1);
// 	assert_(buf != NULL, {
// 		fclose(f);
// 		fprintf(stderr, "Malloc failed\n");
// 	});
// 	assert_(fread(buf, 1, (unsigned long)size, f) == size, { 
// 		fclose(f);
// 		free(buf);
// 		fprintf(stderr, "Failed to read '%s'\n", filepath); 
//     fprintf(stderr, "Reason (errno=%d, '%s')\n", errno, strerror(errno));
// 	});
// 	buf[size] = 0;
// 
// 	fclose(f);
// 	return buf;
// }

void ll_long_string_pushback(long_string_ll* ll, const char* s, size_t len) {
	assert(s && "s cannot be NULL");
	long_string_node* n = (long_string_node*)calloc(1, sizeof(long_string_node));
	assert_(n, {
		fprintf(stderr, "Failed to allocate linked list node\n");
	});
	n->content = s;
	n->length = len;
	if (ll->head == NULL || ll->tail == NULL) {
		ll->head = n;
		ll->tail = n;
	}
	else {
		ll->tail->next = n;
		n->prev = ll->tail;
		ll->tail = n;
	}
}

void ll_long_string_free(long_string_ll* ll) {
	long_string_node* n = ll->head;
	while (n) {
		long_string_node* t = n;
		n = n->next;
		free(t);
	}
	ll->head = 0;
	ll->tail = 0;
}

void ll_long_string_print(long_string_ll* ll) {
	printf("==========================\n");
	printf("\033[0mlong_string_ll print\n");
	long_string_node* n = ll->head;
	//int len = 0;
	while (n) {
		for (int i = 0; i < n->length; i++)
			printf("%c", n->content[i]);
		n = n->next;
		//len++;
	}
	printf("'\n");
	printf("==========================\n");
	printf("\033[0m\n");
}

void ll_long_string_writefile(long_string_ll* ll, FILE* f) {
	long_string_node* n = ll->head;
	while (n) {
		for (int i = 0; i < n->length; i++)
			fprintf(f, "%c", n->content[i]);
		n = n->next;
	}
}

typedef struct generator_context {
	long_string_ll ll;
	const char* embeded[100];
	int embededlen;

	const char* read_contents[100];
	int read_contentslen;
} generator_context;

bool check_has_extension(const char* str, const char* extension) {
	const char* cursor = str;
	while (*cursor && *cursor != '.') cursor++;
	return strncmp(cursor, extension, strlen(extension)) == 0;
}

bool check_embeded(generator_context* ctx, template_file tf) {
	for (int i = 0; i < 10; i++) {
		if (!ctx->embeded[i]) continue;
		if (strcmp(ctx->embeded[i], tf.infilename) == 0) return true;
	}
	return false;
}

char* find_template_path(generator_settings settings, template_file tf) {
	static char path[PATH_MAX];
	struct stat buffer;
	int j = 0;
	for (j = 0; j < settings.path_count; j++) {
		path[0] = 0;
		const char* search_path = settings.search_paths[j];
#ifdef _WIN32
    assert_(_fullpath(path, search_path, _MAX_PATH) != NULL, {
      continue;
    });
#else
    assert_(realpath(search_path, path) != NULL, {
      continue;
    });
#endif
#ifdef _WIN32
		strncat(path, "\\", PATH_MAX);
#else
		strncat(path, "/", PATH_MAX);
#endif
		strncat(path, tf.infilename, PATH_MAX);
		assert_(stat(path, &buffer) == 0, {
			continue;
		})
		break;
	}
	if (j == settings.path_count) return NULL;
	return path;
}

void generator_run_embed(generator_settings settings, ctemplate tplt, template_file tf, replacement repl_, generator_context *ctx, const char* extension, forward_table fwd_table, int depth) {
	if (check_embeded(ctx, tf)) {
		return;
	}

	bool is_root = strcmp(extension, "root") == 0;
	bool has_extension = check_has_extension(tf.infilename, extension);
	if (!is_root && !has_extension) {
		return;
	}

  if (settings.verbose)
    replacement_print(&repl_);
	char* path = find_template_path(settings, tf);
	if (path == NULL) {
		verbose_fprintf(stdout, "\033[33mWarn: Ignoring template file '%s'. Not found.\n\033[0m", tf.infilename);
		return;
	}

	const replacement_item* found = NULL;
	char* content = read_file(path);
	ctx->read_contents[ctx->read_contentslen++]= content;
	const char* cursor = content;
	if (!cursor) {
		verbose_fprintf(stdout, "\033[32mNotice: Something wrong with the read_file of: '%s'\n", path);
		return;
	}

	// add tf to list of embeded files
	ctx->embeded[ctx->embededlen++] = tf.infilename;

	const char* segment_start = (const char*)0;
	const char* segment_end   = (const char*)0;
	int state = 0; // 0 - default, 1 - embed, 2 - replacement
	while (1) {
		if (*cursor == 0) {
			segment_end = cursor;
			ll_long_string_pushback(&ctx->ll, segment_start, segment_end - segment_start);
			break;
		}
		if (state == 0) {
			if (!segment_start) segment_start = cursor;
			if (strncmp(cursor, "#embed", strlen("#embed")) == 0) {
				state = 1;
			}
			else if (strncmp(cursor, "#noembed", strlen("#noembed")) == 0) {
				state = 3;
			}
			else if ((found = replacement_get(&repl_, cursor))) {
				state = 2;
			}
			else {
				state = 0;
				cursor++;
			}
		}
		if (state == 1) { // embed
			segment_end = cursor;
			ll_long_string_pushback(&ctx->ll, segment_start, segment_end - segment_start);
			cursor += strlen("#embed");

			while (*cursor == ' ') cursor++;

			assert_(isdigit(*cursor), {
				fprintf(stderr, "Expected digit after embed\n");
			});

			// parse embed loc
			char* endp = 0;
			int embed_location = strtol(cursor, &endp, 10);
			cursor = endp;
			while (*cursor == ' ') cursor++;

			// parse extension
			const char* extension_begin = cursor;
			while (*cursor != '\n') cursor++;
			char* extension_end = (char*) cursor;

			*extension_end = 0;
			cursor = extension_end + 1;

			// attempt to embed dependencies
			for (int i = 0; i < tplt.deps_count; i++) {
				dependency d = tplt.deps[i];
				if (!d.settings.embed) continue;
				forward_table fwd = d.fwd_table;

				if (d.settings.embedloc == embed_location) {
					for (int j = 0; j < d.template_.template_files_count; j++) {
						template_file tf_ = d.template_.template_files[j];
						replacement rforward = replacement_forward(settings, d.template_.replacement, repl_, fwd);
						generator_run_embed(settings, d.template_, tf_, rforward, ctx, extension_begin, fwd, depth + 1);
						replacement_free(&rforward);
					}
				}
			}

			segment_start = 0;
			state = 0;
		}
		if (state == 2) { // replacement
			segment_end = cursor;
			// if (strcmp(found->needle, "PRINT") == 0) {
			// 	printf("Found PRINT replacement: %.*s\n", 20, cursor);
			// 	printf("Replace with: %s\n", found->with);
			// 	raise(SIGINT);
			// }
			cursor += strlen(found->needle);
			ll_long_string_pushback(&ctx->ll, segment_start, segment_end - segment_start);
			ll_long_string_pushback(&ctx->ll, found->with, strlen(found->with));

			segment_start = 0;
			state = 0;
		}
		if (state == 3) { // no embed
			while (*cursor && *cursor != '\n') cursor++;
			segment_start = 0;
			state = 0;
		}
	}
}

void generator_run(generator_settings settings, ctemplate tplt, replacement repl_) {
	static char outfilepath_realpath[PATH_MAX];
	static char outfilepath_actual[PATH_MAX];

	generator_context ctx = {
		.embeded = {0},
		.embededlen = 0,
		.read_contents = {0},
		.read_contentslen = 0,
	};
	ctx.ll = (long_string_ll){0};
	for (int i = 0; i < tplt.template_files_count; i++) {
		// Figure out where to generate the template to
#ifdef _WIN32
    assert_(_fullpath(outfilepath_realpath, settings.outdir, _MAX_PATH) != NULL, {
			fprintf(stderr, "Failed to realpath. '%s' doesn't exist\n", settings.outdir);
			return;
    });
#else
    const char* dot = strrchr(tplt.template_files[i].outfilename_fmt, '.');
    if (!dot) {
      fprintf(stderr, "Failed to find dot in filename\n");
      continue;
    }
    printf("DOT: %s\n", dot);
    if (strcmp(dot, ".h") == 0) {
      printf("[GENGEN] header generator\n");
      assert_(realpath(settings.header_outdir, outfilepath_realpath) != NULL, {
        fprintf(stderr, "Failed to realpath. '%s' doesn't exist\n", settings.header_outdir);
        return;
      });
    }
    else if (strcmp(dot, ".c") == 0) {
      printf("[GENGEN] source generator\n");
      assert_(realpath(settings.source_outdir, outfilepath_realpath) != NULL, {
        fprintf(stderr, "Failed to realpath. '%s' doesn't exist\n", settings.source_outdir);
        return;
      });
    }
    else {
      strncpy(outfilepath_realpath, "No realpath", PATH_MAX);
    }
#endif
		strncpy(outfilepath_actual, outfilepath_realpath, PATH_MAX);
    printf("%s\n", outfilepath_actual);
#ifdef _WIN32
		strncat(outfilepath_actual, "\\", PATH_MAX);
#else
		strncat(outfilepath_actual, "/", PATH_MAX);
#endif
		const char* cursorfp = tplt.template_files[i].outfilename_fmt;
		const replacement_item* found = NULL;
		while (*cursorfp) {
			if ((found = replacement_get(&repl_, cursorfp))) {
				strncat(outfilepath_actual, found->with, PATH_MAX);
				cursorfp += strlen(found->needle);
			}
			else {
				strncat(outfilepath_actual, cursorfp, 1);
				cursorfp++;
			}
		}

		// Perform generation
		ll_long_string_free(&ctx.ll);
		ctx.embededlen = 0;

		template_file tf = tplt.template_files[i];
		generator_run_embed(settings, tplt, tf, repl_, &ctx, "root", (forward_table){0}, 0);

		if (settings.dryrun) {
			ll_long_string_print(&ctx.ll);
		}
		else {
			FILE* f = fopen(outfilepath_actual, "w");
			assert_(f != NULL, {
				fprintf(stderr, "Couldn't open file '%s'... skipping to next template file...\n", outfilepath_actual);
				continue;
			});
			ll_long_string_writefile(&ctx.ll, f);
			fclose(f);
		}
	}
	for (int i = 0; i < ctx.read_contentslen; i++) {
		free((void*)ctx.read_contents[i]);
	}
	ll_long_string_free(&ctx.ll);
}

#endif
