start: server-start client-start

client-start:
	@echo "start client"
	@$(MAKE) --no-print-directory -C client start

server-start:
	@echo "start server"
	@$(MAKE) --no-print-directory -C server server

server-build:
	@echo "build server"
	@$(MAKE) --no-print-directory -C server all

server-init:
	@echo "init server"
	@$(MAKE) --no-print-directory -C server init
