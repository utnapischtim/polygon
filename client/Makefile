ifneq ($(command -v firefox-bin 2>/dev/null),)
	FIREFOX := firefox-bin
else
	FIREFOX := firefox
endif

start:
	@echo 'firefox: ' $(FIREFOX)
	@$(FIREFOX) index.html
