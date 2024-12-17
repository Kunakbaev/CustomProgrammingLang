
	in
	pop x

	in
	pop y

	whileStart:
	push x
	push 10.00
	lessOrEqualCmp
		push x
		push 10.00
		add
		pop x

	jmp whileStart:
	endWhile:

	push y
	push 4.00
	greaterCmp
	push 0
	je endif:
	push x
	push 3.00
	add
	pop y
	endif:

	push x
	out

	push y
	out

