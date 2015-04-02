if HAVE_DOXYGEN

DOXYGEN_CONF = doxyfile
DOXYGEN_DONE = doxygen.done

$(DOXYGEN_DONE): 
	$(DOXYGEN) $(DOXYGEN_CONF) && echo > $(DOXYGEN_DONE)

all-local: $(DOXYGEN_DONE)

clean-local: 
	$(RM) -rf html

CLEANFILES = $(DOXYGEN_DONE)

endif
