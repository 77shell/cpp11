
/*
 * Pack files into a zip-file
 *
 */

#include <zip.h>
#include <errno.h>
#include <string.h>
#include <gdbm.h>
#include <stdlib.h>

enum zip_error_e {
	eZIP_ok,
	eZIP_err_create_zipfile,
	eZIP_err_mkdir,
	eZIP_err_create_file,
	eZIP_err_add_file,
	eZIP_err_read_file,
	eZIP_err_close_zipfile,
	eZIP_err_invalid_filename,
	eZIP_err_invalid_filep,
	eZIP_err_nofile_exist
};


int
zip(const char *zip_archive, const char *new_file)
{
	if(!new_file) {
		return eZIP_err_invalid_filename;
	}

	int err;
	struct zip *za = zip_open(zip_archive, ZIP_CREATE, &err);
	if(za == nullptr) {
		constexpr size_t buflen {64};
		char errstr[buflen];
		zip_error_to_str(errstr, buflen, err, errno);
		fprintf(stderr, "%s: can't create '%s': %s\n", __func__, zip_archive, errstr);
		return -eZIP_err_create_zipfile;
	}

	{
		zip_error_t err;
		zip_source_t *zs = zip_source_file_create(new_file, 0, 0, &err);
		if(zs == nullptr) {
			zip_close(za);
			return -eZIP_err_create_file;
		}

		if(zip_file_add(za, new_file, zs, ZIP_FL_OVERWRITE | ZIP_FL_ENC_GUESS) == -1) {
			zip_source_free(zs);
			zip_close(za);
			return eZIP_err_add_file;
		}
	}

	zip_close(za);
	return 0;
}

int
zip_filep(const char *zip_archive, FILE *filep, const char *filename)
{
	if(!filep) {
		return eZIP_err_invalid_filep;
	}

	int err;
	struct zip *za = zip_open(zip_archive, ZIP_CREATE, &err);
	if(za == nullptr) {
		constexpr size_t buflen {64};
		char errstr[buflen];
		zip_error_to_str(errstr, buflen, err, errno);
		fprintf(stderr, "%s: can't create '%s': %s\n", __func__, zip_archive, errstr);
		return -eZIP_err_create_zipfile;
	}

	{
		rewind(filep);
		zip_error_t err;
		zip_source_t *zs = zip_source_filep_create(filep, 0, 0, &err);
		if(zs == nullptr) {
			zip_close(za);
			return -eZIP_err_create_file;
		}

		if(zip_file_add(za, filename, zs, ZIP_FL_OVERWRITE | ZIP_FL_ENC_GUESS) == -1) {
			zip_source_free(zs);
			zip_close(za);
			return eZIP_err_add_file;
		}
	}

	zip_close(za);
	return 0;
}

static FILE*
_create_temp_file()
{
	FILE *fp = tmpfile();
	if(fp == NULL)
		perror(__func__);
	return fp;
}

static void
write_random_data(FILE *fp)
{
	char buf[BUFSIZ];
	strcpy(buf, "Hello everyone!");
	size_t w = fwrite(buf, sizeof(char), BUFSIZ, fp);
	printf("Wrote %ld-byte\n", w);
}

static FILE*
dump_dbm(GDBM_FILE dbm, FILE *fp)
{
	int r = gdbm_dump_to_file(dbm, fp, GDBM_DUMP_FMT_ASCII);
	if(r != 0) {
		fprintf(stderr, "%s\n", gdbm_strerror(gdbm_errno));
	}
	return fp;
}

static GDBM_FILE
open_dbm(const char *dbm_path)
{
	GDBM_FILE dbm;
        static const int block_size = 4096;
        if( (dbm = gdbm_open(dbm_path, block_size,
			       GDBM_READER, 0666, nullptr)) == nullptr)
        {
                perror("gdbm_open");
        }
	return dbm;
}


int
main(int argc, char *argv[])
{
	if(argc < 3) {
		fprintf(stderr, "usage: %s zip-file target-file\n", argv[0]);
		return 1;
	}

	// zip(argv[1], argv[2]);

	FILE *fp = _create_temp_file();
	GDBM_FILE dbm = open_dbm("./setpoint.db");
	if(!dbm) {
		exit(0);
	}
	//write_random_data(fp);
	zip_filep(argv[1], fp, argv[2]);
	if(gdbm_close(dbm) != 0) {
		fprintf(stderr, "%s\n", gdbm_strerror(gdbm_errno));
	}
}
