import foo as bar
import mod.bar as foo
import lib

enum Color {
	RED, BLUE, GREEN, WHITE, BLACK
	static var value: int = 0
}

# You cannot do this
# class A : B {}
# class B : C {}
# class C : A {}

abstract class A {
	fun f(){}
	abstract fun b(a:Enum)->int
}
interface B {
	fun b(a:Enum)->int
}
class C : A, B {
	override fun f() {}
	fun b(a:Enum)->int = 0
}

var b_obj: B?
# var test_obj = b_obj?.b(Color.RED) 
var test_obj = b_obj?.b?(Color.RED) 

fun print(*items:any?,/,end:string="\n", sep=" ") {
	# test_obj()
	# test_obj?()

	# while true:
	# 	while false:
	# 		continue
	# 	else:
	# 		break
	# else {
	# 	return
	# 	while false:
	# 		continue
	# 	else:
	# 		continue
	# 	# break
	# }
	var result = "" + items + sep + items + end
	# var sahur = null as? string + "null"
	return
}

class CounterFn {
	private var i = 0

	public init() {}

	public fun __call__() -> int {
		i += 1
		return i - 1
	}
}

const do_count = CounterFn()

fun lambda() {
	var fn1 = fun (i: int) { return i + 1 }
	var fn2 = fun (x: int, y: int): x + y
	var fn3 = fun (): 124124
	var fn4 = fun : 124124
	var fn5 = fun -> int: 124124
	var fn6 = fun { 124124 }

	fn1(1)
	fn2(35, 34)
	fn3()
	fn4() as float
	fn5()
	fn6()
	# var fn = fn1 as int
	# fn1(1)
}

fun main() { 
	var text = "hello, world"
	text = text[::-1] # "dlrow ,olleh"

	var str_buf: lib.StringBuilder? = lib.StringBuilder("west ")
	print(do_count(), do_count(), do_count(), sep: ", ")
	var char = str_buf?[1]
	while false {}
	str_buf?[0] = " "
	while false {}
	(str_buf ??= lib.StringBuilder()) += lib.StringBuilder("bengal\n")
	print(lib.StringBuilder()
		.appendln("hello")
		.appendln("this is from spade")
		.appendln("this text is built from lib.StringBuilder")
		.keep(0, 5)
		.clear()
		.to_string()
	)
	print((0).to_string())
	print("hello, world! my name is aashita", end: "")
	print()
}