cc = g++
prom = test
deps = $(shell find ./ -name "*.h")
src = $(shell find ./ -name "*.cpp")
obj = $(src:%.c=%.o) 

$(prom): $(obj)
	$(cc) -g -rdynamic -o $(prom) $(obj) -lpthread -ldl

%.o: %.c $(deps)
	$(cc) -c $< -o $@

clean:
	rm -rf *.o $(prom) *.log *.txt
