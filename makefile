CC       = clang
SANITIZE = -fsanitize=address -fsanitize=undefined-trap -fsanitize-undefined-trap-on-error
COVERAGE = -fprofile-instr-generate -fcoverage-mapping
OPTS     = $(SANITIZE) $(COVERAGE) -Weverything -Wno-padded -Wno-poison-system-directories
XCRUN    = xcrun

.PHONY : all
all : rfc3339.coverage

%.coverage : %.profdata
	$(XCRUN) llvm-cov show $*.unittest -instr-profile=$< $*.c > $@
	! grep " 0|" $@
	echo PASS $@

%.profdata : %.profraw
	$(XCRUN) llvm-profdata merge -sparse $< -o $@

%.profraw : %.unittest
	LLVM_PROFILE_FILE=$@ ./$<

%.unittest : test_%.c %.c
	$(CC) $(OPTS) $^ -o $@

.PHONY : clean
clean :
	rm -rf *.coverage *.profdata *.profraw *.unittest*
