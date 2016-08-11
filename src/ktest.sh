gcc -o classify runner.c dataframe.c -lm

for i in {1..500};
do
	./classify $i
done

rm classify

python process_ktest.py
