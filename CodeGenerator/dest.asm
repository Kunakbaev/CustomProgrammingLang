
	in
	pop [0]
	in
	pop [1]

	push [0]
	push [1]
	add
	push 10.00
	greaterCmp
	push 0
	je endif1:
		push 10000000.0000000000
		out

	endif1:

	push [0]
	push [1]
	add
	push 10.00
	isEqualCmp
	push 0
	je endif2:
		push -1.0000000000
		out

	endif2:

	push 10.0000000000
	out

