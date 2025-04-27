import foo as bar
import mod.bar as foo
import lib

enum Color {
	RED, BLUE, GREEN, WHITE, BLACK
	static var value: int = 0
}

fun abaxify(parg1:int,*,ac:string){}
fun abaxify(a:int,b:int,c="1"){}
fun abaxify(a,/,b:string,c="a",*args){}

var abaxa=abaxify(a:1,b:2)

class Foo {
	static var size = build()
	public static fun build() -> int {
	}
}

const diddy = Foo.build()

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

fun main() { 
	var str_buf: lib.StringBuilder? = lib.StringBuilder("west ")
	print(do_count(), do_count(), do_count(), sep: ", ")
	while false {}
	(str_buf ??= lib.StringBuilder()) += lib.StringBuilder("bengal\n")
	print(lib.StringBuilder()
		.appendln("hello")
		.appendln("this is from spade")
		.appendln("this text is built from lib.StringBuilder")
		.keep(0, 5)
		.clear()
		.as_string())
	print("hello, world! my name is aashita", end: "")
	print()
}