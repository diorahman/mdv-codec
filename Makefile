mdv_serial_test: mdv_serial_test.c mdv_serial.c
	gcc -g -O0 mdv_serial_test.c mdv_serial.c -o mdv_serial_test

test:
	./mdv_serial_test

clean:
	rm -fr ./mdv_serial_test
