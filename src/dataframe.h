#ifndef DATAFRAME_H
#define DATAFRAME_H

/* for each individual entry (line in file/participant) */
struct ntuple {
	/* data points excluding ID and category */
	double *features;

	/* final category */
	double result;
};

struct dataframe {
	/* limits for indexing loops */
	int numrows;
	int numcols;

	struct ntuple *entries;

	/* columnwise statistical measures
 	 * calculated at init-time POST normalization */
	double *colmeans;
	double *colmedians;
	double *colmaxes;
	double *colmins;
};

/* init dataframe fields */
int init_dataframe(struct dataframe *df, char *filename);

/* free all resources taken by the dataframe */
int free_dataframe(struct dataframe *df);

/* normalize all columns in dataframe */
void normalize_all_cols(struct dataframe *df);

/* normalize values in column to between 0 and 1 */
void normalize_col(struct dataframe *df, int col);

/* fill dataframe from input file
 * ntuples with missing data have been removed from data file
 * https://archive.ics.uci.edu/ml/machine-learning-databases/breast-cancer-wisconsin */
void fill_dataframe(struct dataframe *df, char *filename, char *delimiter);

/* returns the number of lines in a file */
int num_lines_in_file(char *filename);

#endif /* DATAFRAME_H */
