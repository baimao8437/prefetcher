CFLAGS = -msse2 --std gnu99 -O0 -Wall -Wextra

GIT_HOOKS := .git/hooks/applied

EXEC = \
	sse_prefetch_transpose\
	sse_transpose\
	naive_transpose\
	avx_transpose\
	avx_prefetch_transpose

METHOD_NUM = $(words $(EXEC))

REPEAT = 10

TEST = cache-test
ifeq ($(strip $(DIS)),1)
TEST = cache-test-distance
DIS_CASE = \
	0\
	2\
	4\
	8\
	12\
	16
METHOD_NUM = $(words $(DIS_CASE))
endif


all: $(GIT_HOOKS) $(EXEC)

$(GIT_HOOKS):
	@scripts/install-git-hooks
	@echo

%_transpose:
	$(CC) $(CFLAGS) -mavx2 -D$(shell echo $(subst _transpose,,$@) | tr a-z A-Z) -o $@ main.c

run: $(EXEC)
	for method in $(EXEC);do \
		./$$method; \
		printf "\n"; \
	done

cache-test-distance: sse_prefetch_transpose
	>time.txt
	for dis in $(DIS_CASE);do \
		printf "sse_prefetch_"$$dis"_transpose ";\
		perf stat --repeat $(REPEAT) \
		-e cache-misses,cache-references,L1-dcache-load-misses,L1-dcache-loads,L1-dcache-stores,L1-icache-load-misses,r014c,r024c\
		./sse_prefetch_transpose $$dis; \
		printf "\n"; \
	done >> time.txt

cache-test: $(EXEC)
	>time.txt
	for method in $(EXEC);do \
		printf "%s"$$method" ";\
		perf stat --repeat $(REPEAT) \
		-e cache-misses,cache-references,L1-dcache-load-misses,L1-dcache-loads,L1-dcache-stores,L1-icache-load-misses,r014c,r024c\
		./$$method; \
		printf "\n"; \
	done >> time.txt

output.txt: $(TEST) calculate
	./calculate $(METHOD_NUM) $(REPEAT)

plot: output.txt
	gnuplot scripts/runtime.gp

calculate: calculate.c
	$(CC) $(CFLAGS_common) $^ -o $@

clean:
	$(RM) main $(EXEC) *.txt *.gz *.png
