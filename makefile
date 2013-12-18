S=lib Examples
all clean:
	for X in $(S); do (cd $$X; $(MAKE) $@ ); done
