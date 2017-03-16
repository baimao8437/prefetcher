CFLAGS = -msse2 --std gnu99 -O0 -Wall -Wextra

GIT_HOOKS := .git/hooks/applied

EXEC = \
	sse_prefetch_transpose\
	sse_transpose\
	naive_transpose

all: $(GIT_HOOKS) $(EXEC)

$(GIT_HOOKS):
	@scripts/install-git-hooks
	@echo

%_transpose:
	$(CC) $(CFLAGS) -D$(shell echo $(subst _transpose,,$@) | tr a-z A-Z) -o $@ main.c

run: $(EXEC)
	./sse_prefetch_transpose
	./sse_transpose
	./naive_transpose

cache-test: $(EXEC)
	for method in $(EXEC);do \
		perf stat --repeat 100 \
		-e cache-misses,cache-references,L1-dcache-load-misses,L1-dcache-loads,L1-dcache-stores,L1-icache-load-misses \
		./$$method;\
	done
	
clean:
	$(RM) main $(EXEC)
