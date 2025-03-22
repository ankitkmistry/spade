import mod.bar

# var a = b
# var b = c
# var c = a

class Bax {
	# var a = b
	# var b = c
	# var c = a	
}

class A {
	static var a = B.b
}

class B {
	static var b/*: any*/ = A.a
}

fun hello() {
	print("hello foo")
}

# class hello