
all:
	@$(shell (cat Doxyfile; echo "PROJECT_NUMBER=$(XTRACT_VERSION)") | doxygen - )

clean:
	@$(RM) -r latex html
