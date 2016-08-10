for i in {1..500};
do
	./classify $i
done

python process_ktest.py
