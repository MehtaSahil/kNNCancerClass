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
int fill_dataframe(struct dataframe *df, char *filename, char *delimiter);

/* given a string, parse it by delimiter
 * all items EXCEPT the last are stored in tofill
 * the last item is stored in res */
void fill_delimited(char *instr, double *tofill, double *res, char *delimiter);

/* returns the number of lines in a file */
int num_lines_in_file(char *filename);

/* returns all df items in col (zero-indexed) as an array of doubles */
double *df_col_to_arr(struct dataframe *df, int col);

/* STATS */
double double_arr_mean(double *arr, int length);
double double_arr_median(double *arr, int length);
double double_arr_min(double *arr, int length);
double double_arr_max(double *arr, int length);

/* prints nrows ntuples in df to standard output */
int print_n_df_rows(struct dataframe *df, int nrows);

#endif /* DATAFRAME_H */
