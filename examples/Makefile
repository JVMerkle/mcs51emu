all: example.bin example.array.txt

%.hex: %.S
	as31 -Fhex "-O$@" "$<"

%.byte: %.S
	as31 -Fbyte "-O$@" "$<"

%.bin: %.byte byte_to_bin.py
	python3 byte_to_bin.py $< >"$@"
	#xxd "$@"

%.array.txt: %.bin bin_to_array.py
	python3 bin_to_array.py $< >"$@"

.PHONY: clean
clean:
	-rm *.hex *.byte *.bin
