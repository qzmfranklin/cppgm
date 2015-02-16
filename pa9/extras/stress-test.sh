N=0

until [ $? -ne 0 ]; do
	../extras/600-float-calculator-test-data-generator $1 > t
	./600-float-calculator.ref.program < t > x
	../extras/600-float-calculator-cpp-version < t > y
	let N=N+1
	echo $N
	diff x y > /dev/null
done

echo $N

cat t
echo "---cy86---"
cat x
echo "---cpp---"
cat y
echo "---diff---"
diff x y

