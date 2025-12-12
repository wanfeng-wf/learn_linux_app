var = $(wildcard ./*.txt ./test/*.txt)
var1 = $(notdir $(var))
var2 = $(dir $(var))
var3 = $(patsubst %.txt, %.c, $(var))
var4 = $(foreach v, $(var2), $(wildcard $(v)*.txt))

all:
	@echo $(var)
	@echo $(var1)
	@echo $(var2)
	@echo $(var3)
	@echo $(var4)